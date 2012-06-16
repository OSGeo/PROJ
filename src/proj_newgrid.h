/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Definitions related to new datum shift grid (date aware) API.
 * Author:   Frank Warmerdam, <warmerdam@pobox.com>
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

#ifndef PROJ_NEWGRID_H
#define PROJ_NEWGRID_H

typedef struct {
    LP ll;      /* lower left corner coordinates */
    LP del;     /* size of cells */
    ILP lim;    /* width/height of grid (value count) */

    double date; /* year.fraction */

    const char *definition; /* usually the gridname */

    PJ_GRIDINFO  *gridinfo;
    
    int available; /* 0=unknown, 1=true, -1=false */
} PJ_GridCatalogEntry;

typedef struct {
    char *catalog_name;
    
    LP last_grid_ll; /* maximum extent of catalog data */
    LP last_grid_ur;

    int entry_count;
    PJ_GridCatalogEntry *entries;
} PJ_GridCatalog;

typedef struct {

    PJ_GridCatalog *catalog;

    double   datum_date;
    
    PJ_NGRIDINFO *last_before_grid;
    PJ_NGRIDINFO *last_after_grid;
 
    LP last_grid_ll; /* region for which the last grid set is optimal */
    LP last_grid_ur;

} PJ_GridEvalInfo;

PJ_GridCatalog *pj_gc_findcatalog( const char * );
void pj_gc_unloadall();

PJ_GRIDINFO *pj_gc_findgrid( PJ_GridCatalog *catalog, int after, 
                             LP location, double date,
                             LP *optimal_ll, LP *optimal_ur,
                             double *grid_date );


#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_NEWGRID_H */

