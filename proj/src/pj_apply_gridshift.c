/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Apply datum shifts based on grid shift files (normally NAD27 to
 *           NAD83 or the reverse).  This module is responsible for keeping
 *           a list of loaded grids, and calling with each one that is 
 *           allowed for a given datum (expressed as the nadgrids= parameter).
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam <warmerdam@pobox.com>
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

#include <projects.h>
#include <string.h>
#include <math.h>

/************************************************************************/
/*                         pj_apply_gridshift()                         */
/*                                                                      */
/*      This is the externally callable interface - part of the         */
/*      public API - though it is not used internally any more and I    */
/*      doubt it is used by any other applications.  But we preserve    */
/*      it to honour our public api.                                    */
/************************************************************************/

int pj_apply_gridshift( const char *nadgrids, int inverse, 
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    PJ_GRIDINFO **gridlist;
    int           grid_count;
    int           ret;
    
    gridlist = pj_gridlist_from_nadgrids( nadgrids, &grid_count );

    if( gridlist == NULL || grid_count == 0 )
        return pj_errno;

    ret = pj_apply_gridshift_3( gridlist, grid_count, inverse, 
                                point_count, point_offset, x, y, z );

    /* 
    ** Note this frees the array of grid list pointers, but not the grids
    ** which is as intended.  The grids themselves live on.
    */
    pj_dalloc( gridlist );

    return ret;
}

/************************************************************************/
/*                        pj_apply_gridshift_2()                        */
/*                                                                      */
/*      This implmentation takes uses the gridlist from a coordinate    */
/*      system definition.  If the gridlist has not yet been            */
/*      populated in the coordinate system definition we set it up      */
/*      now.                                                            */
/************************************************************************/

int pj_apply_gridshift_2( PJ *defn, int inverse, 
                          long point_count, int point_offset,
                          double *x, double *y, double *z )

{
    if( defn->gridlist == NULL )
    {
        defn->gridlist = 
            pj_gridlist_from_nadgrids( pj_param(defn->params,"snadgrids").s,
                                       &(defn->gridlist_count) );

        if( defn->gridlist == NULL || defn->gridlist_count == 0 )
            return pj_errno;
    }
     
    return pj_apply_gridshift_3( defn->gridlist, defn->gridlist_count, inverse, 
                                 point_count, point_offset, x, y, z );
}


/************************************************************************/
/*                        pj_apply_gridshift_3()                        */
/*                                                                      */
/*      This is the real workhorse, given a gridlist.                   */
/************************************************************************/

int pj_apply_gridshift_3( PJ_GRIDINFO **tables, int grid_count,
                          int inverse, long point_count, int point_offset,
                          double *x, double *y, double *z )

{
    int  i;
    int debug_flag = getenv( "PROJ_DEBUG" ) != NULL;
    static int debug_count = 0;

    if( tables == NULL || grid_count == 0 )
    {
        pj_errno = -38;
        return -38;
    }

    pj_errno = 0;

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;
        LP   input, output;
        int  itable;

        input.phi = y[io];
        input.lam = x[io];
        output.phi = HUGE_VAL;
        output.lam = HUGE_VAL;

        /* keep trying till we find a table that works */
        for( itable = 0; itable < grid_count; itable++ )
        {
            PJ_GRIDINFO *gi = tables[itable];
            struct CTABLE *ct = gi->ct;

            /* skip tables that don't match our point at all.  */
            if( ct->ll.phi > input.phi || ct->ll.lam > input.lam
                || ct->ll.phi + (ct->lim.phi-1) * ct->del.phi < input.phi
                || ct->ll.lam + (ct->lim.lam-1) * ct->del.lam < input.lam )
                continue;

            /* If we have child nodes, check to see if any of them apply. */
            if( gi->child != NULL )
            {
                PJ_GRIDINFO *child;

                for( child = gi->child; child != NULL; child = child->next )
                {
                    struct CTABLE *ct1 = child->ct;

                    if( ct1->ll.phi > input.phi || ct1->ll.lam > input.lam
                      || ct1->ll.phi+(ct1->lim.phi-1)*ct1->del.phi < input.phi
                      || ct1->ll.lam+(ct1->lim.lam-1)*ct1->del.lam < input.lam)
                        continue;

                    break;
                }

                /* we found a more refined child node to use */
                if( child != NULL )
                {
                    gi = child;
                    ct = child->ct;
                }
            }

            /* load the grid shift info if we don't have it. */
            if( ct->cvs == NULL && !pj_gridinfo_load( gi ) )
            {
                pj_errno = -38;
                return -38;
            }
            
            output = nad_cvt( input, inverse, ct );
            if( output.lam != HUGE_VAL )
            {
                if( debug_flag && debug_count++ < 20 )
                    fprintf( stderr,
                             "pj_apply_gridshift(): used %s\n",
                             ct->id );
                break;
            }
        }

        if( output.lam == HUGE_VAL )
        {
            if( debug_flag )
            {
                fprintf( stderr, 
                         "pj_apply_gridshift(): failed to find a grid shift table for\n"
                         "                      location (%.7fdW,%.7fdN)\n",
                         x[io] * RAD_TO_DEG, 
                         y[io] * RAD_TO_DEG );
                for( itable = 0; itable < grid_count; itable++ )
                {
                    PJ_GRIDINFO *gi = tables[itable];
                    if( itable == 0 )
                        fprintf( stderr, "   tried: %s", gi->gridname );
                    else
                        fprintf( stderr, ",%s", gi->gridname );
                }
                fprintf( stderr, "\n" );
            }
                
            pj_errno = PJD_ERR_GRID_AREA;
            return PJD_ERR_GRID_AREA;
        }
        else
        {
            y[io] = output.phi;
            x[io] = output.lam;
        }
    }

    return 0;
}

