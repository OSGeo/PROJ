/******************************************************************************
 *
 * Project:  PROJ.4
 * Purpose:  Mainline program to stress test multithreaded PROJ.4 processing.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
    #include <unistd.h>
#endif

#define num_threads    10
static int num_iterations = 1000000;
static int reinit_every_iteration=0;

typedef struct {
    const char *src_def;
    const char *dst_def;

    PJ_COORD src;
    PJ_COORD dst;

    int     dst_error;
    int     skip;
} TestItem;

static TestItem test_list[] = {
    {
        "+proj=utm +zone=11 +datum=WGS84",
        "+proj=latlong +datum=WGS84",
        proj_coord(150000.0, 3000000.0, 0.0, 0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=utm +zone=11 +datum=NAD83",
        "+proj=latlong +datum=NAD27",
        proj_coord(150000.0, 3000000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=utm +zone=11 +datum=NAD83",
        "+proj=latlong +nadgrids=@null +ellps=WGS84",
        proj_coord(150000.0, 3000000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=utm +zone=11 +datum=WGS84",
        "+proj=merc +datum=potsdam",
        proj_coord(150000.0, 3000000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=latlong +nadgrids=nzgd2kgrid0005.gsb",
        "+proj=latlong +datum=WGS84",
        proj_coord(150000.0, 3000000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=latlong +nadgrids=nzgd2kgrid0005.gsb",
        "+proj=latlong +datum=WGS84",
        proj_coord(170, -40, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=latlong +ellps=GRS80 +towgs84=2,3,5",
        "+proj=latlong +ellps=intl +towgs84=10,12,15",
        proj_coord(170, -40, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=eqc +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        "+proj=stere +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=cea +lat_ts=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=merc +lon_0=12 +k=0.999 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=bonne +lat_1=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=cass +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=nzmg +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=gnom +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=ortho +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=laea +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=aeqd +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=eqdc +lat_1=20 +lat_2=5 +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+proj=mill +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=moll +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        proj_coord(150000.0, 250000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        "+init=epsg:3309",
        "+init=epsg:4326",
        proj_coord(150000.0, 30000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    },
    {
        /* Bad projection (invalid ellipsoid parameter +R_A=0) */
        "+proj=utm +zone=11 +datum=WGS84",
        "+proj=merc +datum=potsdam +R_A=0",
        proj_coord(150000.0, 3000000.0, 0.0, 0.0),
        proj_coord(0.0, 0.0, 0.0, 0.0),
        0, 0
    }
};

static volatile int active_thread_count = 0;

/************************************************************************/
/*                             TestThread()                             */
/************************************************************************/

static void TestThread()

{
    int i, test_count = sizeof(test_list) / sizeof(TestItem);
    int repeat_count = num_iterations;
    int i_iter;

/* -------------------------------------------------------------------- */
/*      Initialize coordinate system definitions.                       */
/* -------------------------------------------------------------------- */
    PJ **pj_list;
    PJ_CONTEXT *ctx = proj_context_create();

    pj_list = (PJ **) calloc(test_count,sizeof(PJ*));

    if(!reinit_every_iteration)
    {
        for( i = 0; i < test_count; i++ )
        {
            TestItem *test = test_list + i;

            pj_list[i] = proj_create_crs_to_crs(
                ctx, test->src_def, test->dst_def, nullptr
            );
        }
    }

/* -------------------------------------------------------------------- */
/*      Perform tests - over and over.                                  */
/* -------------------------------------------------------------------- */

    for( i_iter = 0; i_iter < repeat_count; i_iter++ )
    {
        for( i = 0; i < test_count; i++ )
        {
            TestItem *test = test_list + i;

            if( reinit_every_iteration )
            {
                proj_context_use_proj4_init_rules(nullptr, true);
                pj_list[i] = proj_create_crs_to_crs(
                    ctx, test->src_def, test->dst_def, nullptr
                );

                {
                    int skipTest = (pj_list[i] == nullptr);

                    if ( skipTest != test->skip )
                        fprintf( stderr, "Threaded projection initialization does not match unthreaded initialization\n" );

                    if (skipTest)
                    {
                        proj_destroy( pj_list[i] );
                        continue;
                    }
                }
            }

            if ( test->skip )
                continue;

            PJ_COORD out = proj_trans(pj_list[i], PJ_FWD, test->src);

            int error = proj_errno(pj_list[i]);

            if( error != test->dst_error )
            {
                fprintf( stderr, "Got error %d, expected %d\n",
                         error, test->dst_error );
            }
            proj_errno_reset(pj_list[i]);

            if ( out.xyz.x != test->dst.xyz.x || out.xyz.y != test->dst.xyz.y || out.xyz.z != test->dst.xyz.z)
            //if( x != test->dst_x || y != test->dst_y || z != test->dst_z )
            {
                fprintf( stderr,
                         "Got %.15g,%.15g,%.15g\n"
                         "Expected %.15g,%.15g,%.15g\n"
                         "Diff %.15g,%.15g,%.15g\n",
                         out.xyz.x, out.xyz.y, out.xyz.z,
                         test->dst.xyz.x, test->dst.xyz.y, test->dst.xyz.z,
                         out.xyz.x-test->dst.xyz.x, out.xyz.y-test->dst.xyz.y, out.xyz.z-test->dst.xyz.z);
            }

            if( reinit_every_iteration )
            {
                proj_destroy( pj_list[i] );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
    if( !reinit_every_iteration )
    {
        for( i = 0; i < test_count; i++ )
        {
            proj_destroy( pj_list[i] );
        }
    }

    free( pj_list );

    proj_context_destroy( ctx );

    printf( "%d iterations of the %d tests complete in thread X\n",
            repeat_count, test_count );

    active_thread_count--;
}

#ifdef _WIN32
/************************************************************************/
/*                             WinTestThread()                        */
/************************************************************************/

static DWORD WINAPI WinTestThread( LPVOID lpParameter )

{
    TestThread();

    return 0;
}

#else
/************************************************************************/
/*                             PosixTestThread()                        */
/************************************************************************/

static void *PosixTestThread( void *pData )

{
    (void)pData;
    TestThread();
    return nullptr;
}
#endif

/************************************************************************/
/*                                main()                                */
/************************************************************************/
#ifdef _WIN32
static DWORD WINAPI do_main( LPVOID unused )
#else
static int do_main(void)
#endif
{
/* -------------------------------------------------------------------- */
/*      Our first pass is to establish the correct answers for all      */
/*      the tests.                                                      */
/* -------------------------------------------------------------------- */
    int i, test_count = sizeof(test_list) / sizeof(TestItem);

    for( i = 0; i < test_count; i++ )
    {
        TestItem *test = test_list + i;

        PJ *pj;

        proj_context_use_proj4_init_rules(nullptr, true);
        pj = proj_create_crs_to_crs(
            nullptr, test->src_def, test->dst_def, nullptr
        );

        if( pj == nullptr )
        {
            printf( "Unable to translate:\n%s\n  or\n%s\n", test->src_def, test->dst_def );
            test->skip = 1;
            proj_destroy(pj);
            continue;
        }


        PJ_COORD out = proj_trans(pj, PJ_FWD, test->src);
        test->dst = out;
        test->dst_error  = proj_errno(pj);

        proj_destroy(pj);

        test->skip = 0;

#ifdef nodef
        printf( "Test %d - output %.14g,%.14g,%g\n", i, test->dst.xyz.x, test->dst.xyz.y, test->dst.xyz.z );
#endif
    }

    printf( "%d tests initialized.\n", test_count );

/* -------------------------------------------------------------------- */
/*      Now launch a bunch of threads to repeat the tests.              */
/* -------------------------------------------------------------------- */
#ifdef _WIN32

    { //Scoped to workaround lack of c99 support in VS
        HANDLE ahThread[num_threads];

        for( i = 0; i < num_threads; i++ )
        {
            active_thread_count++;

            ahThread[i] = CreateThread(NULL, 0, WinTestThread, NULL, 0, NULL);

            if (ahThread[i] == 0)
            {
                printf( "Thread creation failed.");
                return 1;
            }
        }

        printf( "%d test threads launched.\n", num_threads );

        WaitForMultipleObjects(num_threads, ahThread, TRUE, INFINITE);
    }

#else
    {
    pthread_t ahThread[num_threads];
    pthread_attr_t hThreadAttr;

    pthread_attr_init( &hThreadAttr );
    pthread_attr_setdetachstate( &hThreadAttr, PTHREAD_CREATE_DETACHED );

    for( i = 0; i < num_threads; i++ )
    {
        active_thread_count++;

        pthread_create( &(ahThread[i]), &hThreadAttr,
            PosixTestThread, nullptr );
    }

    printf( "%d test threads launched.\n", num_threads );

    while( active_thread_count > 0 )
        sleep( 1 );
    }
#endif

    printf( "all tests complete.\n" );

    return 0;
}


int main( int argc, char **argv )
{
    int i;
    for(i=0;i<argc;i++)
    {
        if( strcmp(argv[i], "-reinit") == 0 )
            reinit_every_iteration = 1;
        else if( strcmp(argv[i], "-num_iterations") == 0 && i+1 < argc )
        {
            num_iterations = atoi(argv[i+1]);
            i++;
        }
    }

#ifdef _WIN32
    /* This is an incredible weirdness but with mingw cross-compiler */
    /* 1. - b/a; where double a = 6378206.4; and double b = 6356583.8; */
    /* does not evaluate the same in the main thread or in a thread forked */
    /* by CreateThread(), so run the main in a thread... */
    {
        HANDLE thread = CreateThread(NULL, 0, do_main, NULL, 0, NULL);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle( thread );
    }
#else
    do_main();
#endif
    return 0;
}
