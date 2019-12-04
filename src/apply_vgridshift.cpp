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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include "proj_internal.h"
#include "proj/internal/internal.hpp"

using namespace NS_PROJ;

static double read_vgrid_value( PJ *defn, PJ_LP input, double vmultiplier) {

    /* do not deal with NaN coordinates */
    /* cppcheck-suppress duplicateExpression */
    if( isnan(input.phi) || isnan(input.lam) )
    {
        return HUGE_VAL;
    }

    const VerticalShiftGrid* grid = nullptr;
    for( const auto& gridset: defn->vgrids )
    {
        grid = gridset->gridAt(input.lam, input.phi);
        if( grid )
            break;
    }
    if( !grid )
    {
        return HUGE_VAL;
    }

    const auto& extent = grid->extentAndRes();

    /* Interpolation a location within the grid */
    double grid_x = (input.lam - extent.westLon) / extent.resLon;
    if( extent.fullWorldLongitude() ) {
        // The first fmod goes to ]-lim, lim[ range
        // So we add lim again to be in ]0, 2*lim[ and fmod again
        grid_x = fmod(
            fmod(grid_x + grid->width(), grid->width()) + grid->width(),
            grid->width());
    }
    double grid_y = (input.phi - extent.southLat) / extent.resLat;
    int grid_ix = static_cast<int>(lround(floor(grid_x)));
    assert(grid_ix >= 0 && grid_ix < grid->width());
    int grid_iy = static_cast<int>(lround(floor(grid_y)));
    assert(grid_iy >= 0 && grid_iy < grid->height());
    grid_x -= grid_ix;
    grid_y -= grid_iy;

    int grid_ix2 = grid_ix + 1;
    if( grid_ix2 >= grid->width() ) {
        if( extent.fullWorldLongitude() ) {
            grid_ix2 = 0;
        } else {
            grid_ix2 = grid->width() - 1;
        }
    }
    int grid_iy2 = grid_iy + 1;
    if( grid_iy2 >= grid->height() )
        grid_iy2 = grid->height() - 1;

    float value_a = 0;
    float value_b = 0;
    float value_c = 0;
    float value_d = 0;
    if( !grid->valueAt(grid_ix, grid_iy, value_a) ||
        !grid->valueAt(grid_ix2, grid_iy, value_b) ||
        !grid->valueAt(grid_ix, grid_iy2, value_c) ||
        !grid->valueAt(grid_ix2, grid_iy2, value_d) )
    {
        return HUGE_VAL;
    }

    double total_weight = 0.0;
    int n_weights = 0;
    double value = 0.0f;

    if( !grid->isNodata(value_a, vmultiplier) )
    {
        double weight = (1.0-grid_x) * (1.0-grid_y);
        value += value_a * weight;
        total_weight += weight;
        n_weights ++;
    }
    if( !grid->isNodata(value_b, vmultiplier) )
    {
        double weight = (grid_x) * (1.0-grid_y);
        value += value_b * weight;
        total_weight += weight;
        n_weights ++;
    }
    if( !grid->isNodata(value_c, vmultiplier) )
    {
        double weight = (1.0-grid_x) * (grid_y);
        value += value_c * weight;
        total_weight += weight;
        n_weights ++;
    }
    if( !grid->isNodata(value_d, vmultiplier) )
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

    return value * vmultiplier;
}

/**********************************************/
int proj_vgrid_init(PJ* P, const char *gridkey) {
/**********************************************

  Initizalize and populate gridlist.

    Takes a PJ-object and the plus-parameter
    name that is used in the proj-string to
    specify the grids to load, e.g. "+grids".
    The + should be left out here.

    Returns the number of loaded grids.

***********************************************/

    std::string key("s");
    key += gridkey;
    const char* grids = pj_param(P->ctx, P->params, key.c_str()).s;
    if( grids == nullptr )
        return 0;

    auto listOfGrids = internal::split(std::string(grids), ',');
    for( const auto& grid: listOfGrids )
    {
        const char* gridname = grid.c_str();
        bool canFail = false;
        if( gridname[0] == '@' )
        {
            canFail = true;
            gridname ++;
        }
        auto gridSet = VerticalShiftGridSet::open(P->ctx, gridname);
        if( !gridSet )
        {
            if( !canFail )
            {
                pj_ctx_set_errno( P->ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                P->vgrids.clear();
                return 0;
            }
        }
        else
        {
            P->vgrids.emplace_back(std::move(gridSet));
        }
    }

    return static_cast<int>(P->vgrids.size());
}

/***********************************************/
double proj_vgrid_value(PJ *P, PJ_LP lp, double vmultiplier){
/***********************************************

  Read grid value at position lp in grids loaded
  with proj_grid_init.

  Returns the grid value of the given coordinate.

************************************************/

    double value;

    value = read_vgrid_value(P, lp, vmultiplier);
    proj_log_trace(P, "proj_vgrid_value: (%f, %f) = %f", lp.lam*RAD_TO_DEG, lp.phi*RAD_TO_DEG, value);

    return value;
}
