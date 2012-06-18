/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Code in support of grid catalogs
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2012, Frank Warmerdam <warmerdam@pobox.com>
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

static PJ_GridCatalog *grid_catalog_list = NULL;

/************************************************************************/
/*                          pj_gc_unloadall()                           */
/*                                                                      */
/*      Deallocate all the grid catalogs (but not the referenced        */
/*      grids).                                                         */
/************************************************************************/

void pj_gc_unloadall( projCtx ctx )

{
    while( grid_catalog_list != NULL )
    {
        int i;
        PJ_GridCatalog *catalog = grid_catalog_list;
        grid_catalog_list = grid_catalog_list->next;

        for( i = 0; i < catalog->entry_count; i++ )
        {
            /* we don't own gridinfo - do not free here */
            free( catalog->entries[i].definition );
        }
        free( catalog->entries );
        free( catalog );
    }
}

/************************************************************************/
/*                         pj_gc_findcatalog()                          */
/************************************************************************/

PJ_GridCatalog *pj_gc_findcatalog( projCtx ctx, const char *name )

{
    PJ_GridCatalog *catalog;

    pj_acquire_lock();

    for( catalog=grid_catalog_list; catalog != NULL; catalog = catalog->next ) 
    {
        if( strcmp(catalog->catalog_name, name) == 0 )
        {
            pj_release_lock();
            return catalog;
        }
    }

    pj_release_lock();

    catalog = pj_gc_readcatalog( ctx, name );
    if( catalog == NULL )
        return NULL;

    pj_acquire_lock();
    catalog->next = grid_catalog_list;
    grid_catalog_list = catalog;
    pj_release_lock();

    return catalog;
}

/************************************************************************/
/*                       pj_gc_apply_gridshift()                        */
/************************************************************************/

int pj_gc_apply_gridshift( PJ *defn, int inverse, 
                           long point_count, int point_offset, 
                           double *x, double *y, double *z )

{
    int i;

    if( defn->catalog == NULL ) 
    {
        defn->catalog = pj_gc_findcatalog( defn->ctx, defn->catalog_name );
        if( defn->catalog == NULL )
            return defn->ctx->last_errno;
    }

    ctx->last_errno = 0;

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;
        LP   input, output;
        int  itable;

        input.phi = y[io];
        input.lam = x[io];
        output.phi = HUGE_VAL;
        output.lam = HUGE_VAL;

        /* make sure we have appropriate "before" shift file available */
        if( defn->last_before_grid == NULL
            || input.lam < defn->last_before_region.ll_long
            || input.lam > defn->last_before_region.ur_long
            || input.phi < defn->last_before_region.ll_lat
            || input.phi > defn->last_before_region.ll_lat ) {
            defn->last_before_grid = pj_gc_findgrid(
                defn->ctx, defn->catalog, 0, ll, 0.0, 
                &(defn->last_before_region), NULL);
        }

        PJ_GRIDINFO *gi = defn->last_before_grid;
        struct CTABLE *ct = gi->ct;
        double epsilon = (fabs(ct->del.phi)+fabs(ct->del.lam))/10000.0;
        
        /* If we have child nodes, check to see if any of them apply. */
        if( gi->child != NULL )
        {
            PJ_GRIDINFO *child;
            
            for( child = gi->child; child != NULL; child = child->next )
            {
                struct CTABLE *ct1 = child->ct;
                double epsilon = 
                    (fabs(ct1->del.phi)+fabs(ct1->del.lam))/10000.0;
                
                if( ct1->ll.phi - epsilon > input.phi 
                    || ct1->ll.lam - epsilon > input.lam
                    || (ct1->ll.phi+(ct1->lim.phi-1)*ct1->del.phi + epsilon 
                        < input.phi)
                    || (ct1->ll.lam+(ct1->lim.lam-1)*ct1->del.lam + epsilon 
                        < input.lam) )
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
        if( ct->cvs == NULL && !pj_gridinfo_load( ctx, gi ) )
        {
            pj_ctx_set_errno( ctx, -38 );
            return -38;
        }
            
        output = nad_cvt( input, inverse, ct );
        if( output.lam != HUGE_VAL )
        {
            if( debug_count++ < 20 )
                pj_log( ctx, PJ_LOG_DEBUG_MINOR,
                        "pj_apply_gridshift(): used %s", ct->id );
            break;
        }


        if( output.lam == HUGE_VAL )
        {
            if( ctx->debug_level >= PJ_LOG_DEBUG_MAJOR )
            {
                pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                        "pj_apply_gridshift(): failed to find a grid shift table for\n"
                        "                      location (%.7fdW,%.7fdN)",
                        x[io] * RAD_TO_DEG, 
                        y[io] * RAD_TO_DEG );
                for( itable = 0; itable < grid_count; itable++ )
                {
                    PJ_GRIDINFO *gi = tables[itable];
                    if( itable == 0 )
                        pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                                "   tried: %s", gi->gridname );
                    else
                        pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                                ",%s", gi->gridname );
                }
            }
        }
        else
        {
            y[io] = output.phi;
            x[io] = output.lam;
        }
    }

    return 0;
}
