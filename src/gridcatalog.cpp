/******************************************************************************
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

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"

static PJ_GridCatalog *grid_catalog_list = nullptr;

static
PJ_GRIDINFO *pj_gc_findgrid( projCtx_t *ctx,
                             PJ_GridCatalog *catalog, int after,
                             PJ_LP location, double date,
                             PJ_Region *optional_region,
                             double *grid_date );

/************************************************************************/
/*                          pj_gc_unloadall()                           */
/*                                                                      */
/*      Deallocate all the grid catalogs (but not the referenced        */
/*      grids).                                                         */
/************************************************************************/

void pj_gc_unloadall( projCtx ctx )
{
    (void) ctx;

    while( grid_catalog_list != nullptr )
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
        free( catalog->catalog_name );
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

    for( catalog=grid_catalog_list; catalog != nullptr; catalog = catalog->next ) 
    {
        if( strcmp(catalog->catalog_name, name) == 0 )
        {
            pj_release_lock();
            return catalog;
        }
    }

    pj_release_lock();

    catalog = pj_gc_readcatalog( ctx, name );
    if( catalog == nullptr )
        return nullptr;

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
    (void) z;

    if( defn->catalog == nullptr ) 
    {
        defn->catalog = pj_gc_findcatalog( defn->ctx, defn->catalog_name );
        if( defn->catalog == nullptr )
            return defn->ctx->last_errno;
    }

    defn->ctx->last_errno = 0;

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;
        PJ_LP   input, output_after, output_before;
        double mix_ratio;
        PJ_GRIDINFO *gi;

        input.phi = y[io];
        input.lam = x[io];

        /* make sure we have appropriate "after" shift file available */
        if( defn->last_after_grid == nullptr
            || input.lam < defn->last_after_region.ll_long
            || input.lam > defn->last_after_region.ur_long
            || input.phi < defn->last_after_region.ll_lat
            || input.phi > defn->last_after_region.ll_lat ) {
            defn->last_after_grid = 
                pj_gc_findgrid( defn->ctx, defn->catalog, 
                                1, input, defn->datum_date, 
                                &(defn->last_after_region), 
                                &(defn->last_after_date));
            if( defn->last_after_grid == nullptr )
            {
                pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                return PJD_ERR_FAILED_TO_LOAD_GRID;
            }
        }
        gi = defn->last_after_grid;
        assert( gi->child == nullptr );

        /* load the grid shift info if we don't have it. */
        if( gi->ct->cvs == nullptr && !pj_gridinfo_load( defn->ctx, gi ) )
        {
            pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return PJD_ERR_FAILED_TO_LOAD_GRID;
        }
            
        output_after = nad_cvt( input, inverse, gi->ct );
        if( output_after.lam == HUGE_VAL )
        {
            if( defn->ctx->debug_level >= PJ_LOG_DEBUG_MAJOR )
            {
                pj_log( defn->ctx, PJ_LOG_DEBUG_MAJOR,
                        "pj_apply_gridshift(): failed to find a grid shift table for\n"
                        "                      location (%.7fdW,%.7fdN)",
                        x[io] * RAD_TO_DEG, 
                        y[io] * RAD_TO_DEG );
            }
            continue;
        }

        if( defn->datum_date == 0.0 ) 
        {
            y[io] = output_after.phi;
            x[io] = output_after.lam;
            continue;
        }

        /* make sure we have appropriate "before" shift file available */
        if( defn->last_before_grid == nullptr
            || input.lam < defn->last_before_region.ll_long
            || input.lam > defn->last_before_region.ur_long
            || input.phi < defn->last_before_region.ll_lat
            || input.phi > defn->last_before_region.ll_lat ) {
            defn->last_before_grid = 
                pj_gc_findgrid( defn->ctx, defn->catalog, 
                                0, input, defn->datum_date, 
                                &(defn->last_before_region), 
                                &(defn->last_before_date));
            if( defn->last_before_grid == nullptr )
            {
                pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                return PJD_ERR_FAILED_TO_LOAD_GRID;
            }
        }

        gi = defn->last_before_grid;
        assert( gi->child == nullptr );

        /* load the grid shift info if we don't have it. */
        if( gi->ct->cvs == nullptr && !pj_gridinfo_load( defn->ctx, gi ) )
        {
            pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return PJD_ERR_FAILED_TO_LOAD_GRID;
        }
            
        output_before = nad_cvt( input, inverse, gi->ct );
        if( output_before.lam == HUGE_VAL )
        {
            if( defn->ctx->debug_level >= PJ_LOG_DEBUG_MAJOR )
            {
                pj_log( defn->ctx, PJ_LOG_DEBUG_MAJOR,
                        "pj_apply_gridshift(): failed to find a grid shift table for\n"
                        "                      location (%.7fdW,%.7fdN)",
                        x[io] * RAD_TO_DEG, 
                        y[io] * RAD_TO_DEG );
            }
            continue;
        }

        mix_ratio = (defn->datum_date - defn->last_before_date) 
            / (defn->last_after_date - defn->last_before_date);

        y[io] = mix_ratio * output_after.phi 
            + (1.0-mix_ratio) * output_before.phi;
        x[io] = mix_ratio * output_after.lam 
            + (1.0-mix_ratio) * output_before.lam;
    }

    return 0;
}

/************************************************************************/
/*                           pj_c_findgrid()                            */
/************************************************************************/

static
PJ_GRIDINFO *pj_gc_findgrid( projCtx ctx, PJ_GridCatalog *catalog, int after,
                             PJ_LP location, double date,
                             PJ_Region *optional_region,
                             double *grid_date ) 
{
    int iEntry;
    PJ_GridCatalogEntry *entry = nullptr;

    for( iEntry = 0; iEntry < catalog->entry_count; iEntry++ ) 
    {
        entry = catalog->entries + iEntry;

        if( (after && entry->date < date) 
            || (!after && entry->date > date) )
            continue;

        if( location.lam < entry->region.ll_long
            || location.lam > entry->region.ur_long
            || location.phi < entry->region.ll_lat
            || location.phi > entry->region.ur_lat )
            continue;

        if( entry->available == -1 )
            continue;

        break;
    }

    if( entry == nullptr )
    {
        if( grid_date )
            *grid_date = 0.0;
        if( optional_region != nullptr )
            memset( optional_region, 0, sizeof(PJ_Region));
        return nullptr;
    }

    if( grid_date )
        *grid_date = entry->date;

    if( optional_region )
    {
        
    }

    if( entry->gridinfo == nullptr )
    {
        PJ_GRIDINFO **gridlist = nullptr;
        int grid_count = 0;
        gridlist = pj_gridlist_from_nadgrids( ctx, entry->definition, 
                                              &grid_count);
        // FIXME: this leaks gridlist itself, and memory ownership of
        // entry->gridinfo is also confusing. Coverity CID 193539
        if( grid_count == 1 )
            entry->gridinfo = gridlist[0];
    }
    
    return entry->gridinfo;
}
                             
