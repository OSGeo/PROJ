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
#include <pthread.h>
#include <unistd.h>
#include "proj_api.h"

#define num_threads    10
#define num_iterations 1000000
#define reinit_every_iteration 0

typedef struct {
    const char *src_def;
    const char *dst_def;

    double  src_x, src_y, src_z;
    double  dst_x, dst_y, dst_z;
    
    int     dst_error;
    int     skip;
    

} TestItem; 

TestItem test_list[] = {
    {
        "+proj=utm +zone=11 +datum=WGS84",
        "+proj=latlong +datum=WGS84",
        150000.0, 3000000.0, 0.0,
    },
    {
        "+proj=utm +zone=11 +datum=NAD83",
        "+proj=latlong +datum=NAD27",
        150000.0, 3000000.0, 0.0,  
    },
    {
        "+proj=utm +zone=11 +datum=NAD83",
        "+proj=latlong +nadgrids=@null +ellps=WGS84",
        150000.0, 3000000.0, 0.0,
    },
    {
        "+proj=utm +zone=11 +datum=WGS84",
        "+proj=merc +datum=potsdam",
        150000.0, 3000000.0, 0.0,
    },
    {
        "+proj=latlong +nadgrids=nzgd2kgrid0005.gsb",
        "+proj=latlong +datum=WGS84",
        150000.0, 3000000.0, 0.0,
    },
    {
        "+proj=latlong +nadgrids=nzgd2kgrid0005.gsb",
        "+proj=latlong +datum=WGS84",
        170 * DEG_TO_RAD, -40 * DEG_TO_RAD, 0.0,
    },
    {
        "+proj=latlong +ellps=GRS80 +towgs84=2,3,5",
        "+proj=latlong +ellps=intl +towgs84=10,12,15",
        170 * DEG_TO_RAD, -40 * DEG_TO_RAD, 0.0,
    },
    {
        "+proj=eqc +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        "+proj=stere +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=cea +lat_ts=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=merc +lon_0=12 +k=0.999 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=bonne +lat_1=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=cass +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=nzmg +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=gnom +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=ortho +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=laea +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=aeqd +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=eqdc +lat_1=20 +lat_2=5 +lat_0=11 +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+proj=mill +lat_0=11 +lon_0=12 +y_0=200000 +datum=WGS84 ",
        "+proj=moll +lon_0=12 +x_0=100000 +y_0=200000 +datum=WGS84 ",
        150000.0, 250000.0, 0.0,
    },
    {
        "+init=epsg:3309",
        "+init=epsg:4326",
        150000.0, 30000.0, 0.0,
    }
};

static volatile int active_thread_count = 0;

/************************************************************************/
/*                             TestThread()                             */
/************************************************************************/

static void *TestThread( void *pData )

{
    int i, test_count = sizeof(test_list) / sizeof(TestItem); 

/* -------------------------------------------------------------------- */
/*      Initialize coordinate system definitions.                       */
/* -------------------------------------------------------------------- */
    projPJ *src_pj_list, *dst_pj_list;
    projCtx ctx = pj_ctx_alloc();
//    projCtx ctx = pj_get_default_ctx();
    
    src_pj_list = (projPJ *) calloc(test_count,sizeof(projPJ));
    dst_pj_list = (projPJ *) calloc(test_count,sizeof(projPJ));
                                
#if reinit_every_iteration == 0
    for( i = 0; i < test_count; i++ )
    {
        TestItem *test = test_list + i;

        src_pj_list[i] = pj_init_plus_ctx( ctx, test->src_def );
        dst_pj_list[i] = pj_init_plus_ctx( ctx, test->dst_def );
    }
#endif
    
/* -------------------------------------------------------------------- */
/*      Perform tests - over and over.                                  */
/* -------------------------------------------------------------------- */
    int repeat_count = num_iterations, i_iter;
    
    for( i_iter = 0; i_iter < repeat_count; i_iter++ )
    {
        for( i = 0; i < test_count; i++ )
        {
            TestItem *test = test_list + i;
            double x, y, z;
            int error;

            if( test->skip )
                continue;
            
            x = test->src_x;
            y = test->src_y;
            z = test->src_z;

#if reinit_every_iteration == 1
            src_pj_list[i] = pj_init_plus_ctx( ctx, test->src_def );
            dst_pj_list[i] = pj_init_plus_ctx( ctx, test->dst_def );
#endif

            error = pj_transform( src_pj_list[i], dst_pj_list[i], 1, 0, 
                                  &x, &y, &z );
            

            if( error != test->dst_error )
            {
                fprintf( stderr, "Got error %d, expected %d\n", 
                         error, test->dst_error );
            }

            if( x != test->dst_x || y != test->dst_y || z != test->dst_z )
            {
                fprintf( stderr, 
                         "Got %.15g,%.15g,%.15g\n"
                         "Expected %.15g,%.15g,%.15g\n",
                         x, y, z, 
                         test->dst_x, test->dst_y, test->dst_z );
            }

#if reinit_every_iteration == 1
            pj_free( src_pj_list[i] );
            pj_free( dst_pj_list[i] );
#endif
        }
    }

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
#if reinit_every_iteration == 0
    for( i = 0; i < test_count; i++ )
    {
        pj_free( src_pj_list[i] );
        pj_free( dst_pj_list[i] );
    }
#endif
    
    free( src_pj_list );
    free( dst_pj_list );

    pj_ctx_free( ctx );

    printf( "%d iterations of the %d tests complete in thread X\n", 
            repeat_count, test_count );

    active_thread_count--;

    return NULL;
}

/************************************************************************/
/*                                main()                                */
/************************************************************************/
int main( int argc, char **argv )

{
/* -------------------------------------------------------------------- */
/*      Our first pass is to establish the correct answers for all      */
/*      the tests.                                                      */
/* -------------------------------------------------------------------- */
    int i, test_count = sizeof(test_list) / sizeof(TestItem); 

    for( i = 0; i < test_count; i++ )
    {
        TestItem *test = test_list + i;

        projPJ src_pj, dst_pj;

        src_pj = pj_init_plus( test->src_def );
        dst_pj = pj_init_plus( test->dst_def );

        if( src_pj == NULL )
        {
            printf( "Unable to translate:\n%s\n", test->src_def );
            test->skip = 1;
            continue;
        }

        if( dst_pj == NULL )
        {
            printf( "Unable to translate:\n%s\n", test->dst_def );
            test->skip = 1;
            continue;
        }
        
        test->dst_x = test->src_x;
        test->dst_y = test->src_y;
        test->dst_z = test->src_z;

        test->dst_error = pj_transform( src_pj, dst_pj, 1, 0, 
                                        &(test->dst_x), 
                                        &(test->dst_y),
                                        &(test->dst_z) );
     
        pj_free( src_pj );
        pj_free( dst_pj );

        test->skip = 0;

#ifdef notdef
        printf( "Test %d - output %.14g,%.14g,%g\n", i, test->dst_x, test->dst_y, test->dst_z );
#endif
    }

    printf( "%d tests initialized.\n", test_count );

/* -------------------------------------------------------------------- */
/*      Now launch a bunch of threads to repeat the tests.              */
/* -------------------------------------------------------------------- */
    pthread_t ahThread[num_threads];
    pthread_attr_t hThreadAttr;

    pthread_attr_init( &hThreadAttr );
    pthread_attr_setdetachstate( &hThreadAttr, PTHREAD_CREATE_DETACHED );

    for( i = 0; i < num_threads; i++ )
    {
        active_thread_count++;
        
        pthread_create( &(ahThread[i]), &hThreadAttr, 
                        TestThread, NULL );
    }

    printf( "%d test threads launched.\n", num_threads );
            
    while( active_thread_count > 0 )				       
        sleep( 1 );

    printf( "all tests complete.\n" );

    return 0;
}
