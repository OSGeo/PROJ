/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Apply vertical datum shifts based on grid shift files, normally
 *           geoid grids mapping WGS84 to NAVD88 or something similar.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2010, Frank Warmerdam <warmerdam@pobox.com>
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

#include <stdio.h>
#include <string.h>

#include "proj_math.h"
#include "proj_internal.h"
#include "projects.h"

static int is_nodata(float value)
{
    /* nodata?  */
    /* GTX official nodata value if  -88.88880f, but some grids also */
    /* use other  big values for nodata (e.g naptrans2008.gtx has */
    /* nodata values like -2147479936), so test them too */
    return value > 1000 || value < -1000 || value == -88.88880f;
}

static double read_vgrid_value( PJ *defn, LP input, int *gridlist_count_p, PJ_GRIDINFO **tables, struct CTABLE *ct) {
    int  itable = 0;
    double value = HUGE_VAL;
    double grid_x, grid_y;
    long   grid_ix, grid_iy;
    long   grid_ix2, grid_iy2;
    float  *cvs;
    /* do not deal with NaN coordinates */
    /* cppcheck-suppress duplicateExpression */
    if( isnan(input.phi) || isnan(input.lam) )
        itable = *gridlist_count_p;

    /* keep trying till we find a table that works */
    for ( ; itable < *gridlist_count_p; itable++ )
    {
        PJ_GRIDINFO *gi = tables[itable];

        ct = gi->ct;

        /* skip tables that don't match our point at all.  */
        if( ct->ll.phi > input.phi || ct->ll.lam > input.lam
            || ct->ll.phi + (ct->lim.phi-1) * ct->del.phi < input.phi
            || ct->ll.lam + (ct->lim.lam-1) * ct->del.lam < input.lam )
            continue;

        /* If we have child nodes, check to see if any of them apply. */
        while( gi->child != NULL )
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

            /* we didn't find a more refined child node to use, so go with current grid */
            if( child == NULL )
            {
                break;
            }

            /* Otherwise let's try for childrens children .. */
            gi = child;
            ct = child->ct;
        }

        /* load the grid shift info if we don't have it. */
        if( ct->cvs == NULL && !pj_gridinfo_load( pj_get_ctx(defn), gi ) )
        {
            pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return PJD_ERR_FAILED_TO_LOAD_GRID;
        }


        /* Interpolation a location within the grid */
        grid_x = (input.lam - ct->ll.lam) / ct->del.lam;
        grid_y = (input.phi - ct->ll.phi) / ct->del.phi;
        grid_ix = lround(floor(grid_x));
        grid_iy = lround(floor(grid_y));
        grid_x -= grid_ix;
        grid_y -= grid_iy;

        grid_ix2 = grid_ix + 1;
        if( grid_ix2 >= ct->lim.lam )
            grid_ix2 = ct->lim.lam - 1;
        grid_iy2 = grid_iy + 1;
        if( grid_iy2 >= ct->lim.phi )
            grid_iy2 = ct->lim.phi - 1;

        cvs = (float *) ct->cvs;
        {
            float value_a = cvs[grid_ix + grid_iy * ct->lim.lam];
            float value_b = cvs[grid_ix2 + grid_iy * ct->lim.lam];
            float value_c = cvs[grid_ix + grid_iy2 * ct->lim.lam];
            float value_d = cvs[grid_ix2 + grid_iy2 * ct->lim.lam];
            double total_weight = 0.0;
            int n_weights = 0;
            value = 0.0f;
            if( !is_nodata(value_a) )
            {
                double weight = (1.0-grid_x) * (1.0-grid_y);
                value += value_a * weight;
                total_weight += weight;
                n_weights ++;
            }
            if( !is_nodata(value_b) )
            {
                double weight = (grid_x) * (1.0-grid_y);
                value += value_b * weight;
                total_weight += weight;
                n_weights ++;
            }
            if( !is_nodata(value_c) )
            {
                double weight = (1.0-grid_x) * (grid_y);
                value += value_c * weight;
                total_weight += weight;
                n_weights ++;
            }
            if( !is_nodata(value_d) )
            {
                double weight = (grid_x) * (grid_y);
                value += value_d * weight;
                total_weight += weight;
                n_weights ++;
            }
            if( n_weights == 0 )
                value = HUGE_VAL;
            else if( n_weights != 4 )
                value /= total_weight;
        }

    }

    return value;
}

/************************************************************************/
/*                        pj_apply_vgridshift()                         */
/*                                                                      */
/*      This implementation takes uses the gridlist from a coordinate   */
/*      system definition.  If the gridlist has not yet been            */
/*      populated in the coordinate system definition we set it up      */
/*      now.                                                            */
/************************************************************************/
int pj_apply_vgridshift( PJ *defn, const char *listname,
                         PJ_GRIDINFO ***gridlist_p,
                         int *gridlist_count_p,
                         int inverse,
                         long point_count, int point_offset,
                         double *x, double *y, double *z )

{
    int  i;
    static int debug_count = 0;
    PJ_GRIDINFO **tables;
    struct CTABLE ct;

    if( *gridlist_p == NULL )
    {
        *gridlist_p =
            pj_gridlist_from_nadgrids( pj_get_ctx(defn),
                                       pj_param(defn->ctx,defn->params,listname).s,
                                       gridlist_count_p );

        if( *gridlist_p == NULL || *gridlist_count_p == 0 )
            return defn->ctx->last_errno;
    }

    if( *gridlist_count_p == 0 )
    {
        pj_ctx_set_errno( defn->ctx, PJD_ERR_FAILED_TO_LOAD_GRID);
        return PJD_ERR_FAILED_TO_LOAD_GRID;
    }

    tables = *gridlist_p;
    defn->ctx->last_errno = 0;

    for( i = 0; i < point_count; i++ )
    {
        double value;
        long io = i * point_offset;
        LP   input;

        input.phi = y[io];
        input.lam = x[io];

        value = read_vgrid_value(defn, input, gridlist_count_p, tables, &ct);

        if( inverse )
            z[io] -= value;
        else
            z[io] += value;
        if( value != HUGE_VAL )
        {
            if( debug_count++ < 20 ) {
                proj_log_trace(defn, "pj_apply_gridshift(): used %s", ct.id);
                break;
            }
        }

        if( value == HUGE_VAL )
        {
            int itable;
            char gridlist[3000];

            proj_log_debug(defn,
                "pj_apply_vgridshift(): failed to find a grid shift table for\n"
                "                       location (%.7fdW,%.7fdN)",
                x[io] * RAD_TO_DEG,
                y[io] * RAD_TO_DEG );

            gridlist[0] = '\0';
            for( itable = 0; itable < *gridlist_count_p; itable++ )
            {
                PJ_GRIDINFO *gi = tables[itable];
                if( strlen(gridlist) + strlen(gi->gridname) > sizeof(gridlist)-100 )
                {
                    strcat( gridlist, "..." );
                    break;
                }

                if( itable == 0 )
                    sprintf( gridlist, "   tried: %s", gi->gridname );
                else
                    sprintf( gridlist+strlen(gridlist), ",%s", gi->gridname );
            }

            proj_log_debug(defn, "%s", gridlist);
            pj_ctx_set_errno( defn->ctx, PJD_ERR_GRID_AREA );

            return PJD_ERR_GRID_AREA;
        }
    }

    return 0;
}

/**********************************************/
int proj_vgrid_init(PJ* P, const char *grids) {
/**********************************************

  Initizalize and populate gridlist.

    Takes a PJ-object and the plus-parameter
    name that is used in the proj-string to
    specify the grids to load, e.g. "+grids".
    The + should be left out here.

    Returns the number of loaded grids.

***********************************************/

    /* prepend "s" to the "grids" string to allow usage with pj_param */
    char *sgrids = (char *) pj_malloc( (strlen(grids)+1+1) *sizeof(char) );
    sprintf(sgrids, "%s%s", "s", grids);

    if (P->vgridlist_geoid == NULL) {
        P->vgridlist_geoid = pj_gridlist_from_nadgrids(
            P->ctx,
            pj_param(P->ctx, P->params, sgrids).s,
            &(P->vgridlist_geoid_count)
        );

        if( P->vgridlist_geoid == NULL || P->vgridlist_geoid_count == 0 ) {
            pj_dealloc(sgrids);
            return 0;
        }
    }

    if (P->vgridlist_geoid_count == 0) {
        proj_errno_set(P, PJD_ERR_FAILED_TO_LOAD_GRID);
    }

    pj_dealloc(sgrids);
    return P->vgridlist_geoid_count;
}

/***********************************************/
double proj_vgrid_value(PJ *P, LP lp){
/***********************************************

  Read grid value at position lp in grids loaded
  with proj_grid_init.

  Returns the grid value of the given coordinate.

************************************************/

    struct CTABLE used_grid;
    double value;
    memset(&used_grid, 0, sizeof(struct CTABLE));

    value = read_vgrid_value(P, lp, &(P->vgridlist_geoid_count), P->vgridlist_geoid, &used_grid);
    proj_log_trace(P, "proj_vgrid_value: (%f, %f) = %f", lp.lam*RAD_TO_DEG, lp.phi*RAD_TO_DEG, value);

    return value;
}
