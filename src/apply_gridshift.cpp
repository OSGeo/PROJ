/******************************************************************************
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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "proj_internal.h"
#include "proj/internal/internal.hpp"
#include "grids.hpp"

NS_PROJ_START

// ---------------------------------------------------------------------------

static const HorizontalShiftGrid* findGrid(const ListOfHGrids& grids,
                                           PJ_LP input)
{
    for( const auto& gridset: grids )
    {
        auto grid = gridset->gridAt(input.lam, input.phi);
        if( grid )
            return grid;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

static ListOfHGrids getListOfGridSets(PJ_CONTEXT* ctx, const char* grids)
{
    ListOfHGrids list;
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
        auto gridSet = HorizontalShiftGridSet::open(ctx, gridname);
        if( !gridSet )
        {
            if( !canFail )
            {
                pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                return {};
            }
        }
        else
        {
            list.emplace_back(std::move(gridSet));
        }
    }
    return list;
}


/**********************************************/
ListOfHGrids proj_hgrid_init(PJ* P, const char *gridkey) {
/**********************************************

  Initizalize and populate list of horizontal
  grids.

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
        return {};

    return getListOfGridSets(P->ctx, grids);
}

typedef struct { pj_int32 lam, phi; } ILP;

static PJ_LP nad_intr(PJ_LP t, const HorizontalShiftGrid* grid) {
	PJ_LP val, frct;
	ILP indx;
	int in;

        const auto& extent = grid->extentAndRes();
	t.lam /= extent.resLon;
	indx.lam = isnan(t.lam) ? 0 : (pj_int32)lround(floor(t.lam));
	t.phi /= extent.resLat;
	indx.phi = isnan(t.phi) ? 0 : (pj_int32)lround(floor(t.phi));

	frct.lam = t.lam - indx.lam;
	frct.phi = t.phi - indx.phi;
	val.lam = val.phi = HUGE_VAL;
	if (indx.lam < 0) {
		if (indx.lam == -1 && frct.lam > 0.99999999999) {
			++indx.lam;
			frct.lam = 0.;
		} else
			return val;
	} else if ((in = indx.lam + 1) >= grid->width()) {
		if (in == grid->width() && frct.lam < 1e-11) {
			--indx.lam;
			frct.lam = 1.;
		} else
			return val;
	}
	if (indx.phi < 0) {
		if (indx.phi == -1 && frct.phi > 0.99999999999) {
			++indx.phi;
			frct.phi = 0.;
		} else
			return val;
	} else if ((in = indx.phi + 1) >= grid->height()) {
		if (in == grid->height() && frct.phi < 1e-11) {
			--indx.phi;
			frct.phi = 1.;
		} else
			return val;
	}
	
	float f00Lon = 0, f00Lat = 0;
        float f10Lon = 0, f10Lat = 0;
        float f01Lon = 0, f01Lat = 0;
        float f11Lon = 0, f11Lat = 0;
	if( !grid->valueAt(indx.lam, indx.phi, f00Lon, f00Lat) ||
            !grid->valueAt(indx.lam + 1, indx.phi, f10Lon, f10Lat) ||
            !grid->valueAt(indx.lam, indx.phi + 1, f01Lon, f01Lat) ||
            !grid->valueAt(indx.lam + 1, indx.phi + 1, f11Lon, f11Lat) )
        {
            return val;
        }

        double m10 = frct.lam;
	double m11 = m10;
        double m01 = 1. - frct.lam;
	double m00 = m01;
	m11 *= frct.phi;
	m01 *= frct.phi;
	frct.phi = 1. - frct.phi;
	m00 *= frct.phi;
	m10 *= frct.phi;
	val.lam = m00 * f00Lon + m10 * f10Lon +
			  m01 * f01Lon + m11 * f11Lon;
	val.phi = m00 * f00Lat + m10 * f10Lat +
			  m01 * f01Lat + m11 * f11Lat;
	return val;
}


#define MAX_ITERATIONS 10
#define TOL 1e-12

static
PJ_LP nad_cvt(PJ_LP in, int inverse, const HorizontalShiftGrid* grid) {
    PJ_LP t, tb,del, dif;
    int i = MAX_ITERATIONS;
    const double toltol = TOL*TOL;

    if (in.lam == HUGE_VAL)
        return in;

    /* normalize input to ll origin */
    tb = in;
    const auto& extent = grid->extentAndRes();
    tb.lam -= extent.westLon;
    tb.phi -= extent.southLat;

    tb.lam = adjlon (tb.lam - M_PI) + M_PI;

    t = nad_intr (tb, grid);
    if (t.lam == HUGE_VAL)
        return t;

    if (!inverse) {
        in.lam -= t.lam;
        in.phi += t.phi;
        return in;
    }

    t.lam = tb.lam + t.lam;
    t.phi = tb.phi - t.phi;

    do {
        del = nad_intr(t, grid);

        /* This case used to return failure, but I have
           changed it to return the first order approximation
           of the inverse shift.  This avoids cases where the
           grid shift *into* this grid came from another grid.
           While we aren't returning optimally correct results
           I feel a close result in this case is better than
           no result.  NFW
           To demonstrate use -112.5839956 49.4914451 against
           the NTv2 grid shift file from Canada. */
        if (del.lam == HUGE_VAL)
            break;

        dif.lam = t.lam - del.lam - tb.lam;
           dif.phi = t.phi + del.phi - tb.phi;
        t.lam -= dif.lam;
        t.phi -= dif.phi;

    } while (--i && (dif.lam*dif.lam + dif.phi*dif.phi > toltol)); /* prob. slightly faster than hypot() */

    if (i==0) {
        /* If we had access to a context, this should go through pj_log, and we should set ctx->errno */
        if (getenv ("PROJ_DEBUG"))
            fprintf( stderr, "Inverse grid shift iterator failed to converge.\n" );
        t.lam = t.phi = HUGE_VAL;
        return t;
    }

    /* and again: pj_log and ctx->errno */
    if (del.lam==HUGE_VAL && getenv ("PROJ_DEBUG"))
        fprintf (stderr, "Inverse grid shift iteration failed, presumably at grid edge.\nUsing first approximation.\n");

    in.lam = adjlon (t.lam + extent.westLon);
    in.phi = t.phi + extent.southLat;
    return in;
}

/********************************************/
/*           proj_hgrid_value()             */
/*                                          */
/*    Return coordinate offset in grid      */
/********************************************/
PJ_LP proj_hgrid_value(PJ *P, const ListOfHGrids& grids, PJ_LP lp) {
    PJ_LP out = proj_coord_error().lp;

    const auto grid = findGrid(grids, lp);
    if( !grid ) {
        pj_ctx_set_errno( P->ctx, PJD_ERR_GRID_AREA );
        return out;
    }

    /* normalize input to ll origin */
    const auto& extent = grid->extentAndRes();
    lp.lam -= extent.westLon;
    lp.phi -= extent.southLat;

    lp.lam = adjlon(lp.lam - M_PI) + M_PI;

    out = nad_intr(lp, grid);

    if (out.lam == HUGE_VAL || out.phi == HUGE_VAL) {
        pj_ctx_set_errno(P->ctx, PJD_ERR_GRID_AREA);
    }

    return out;
}

static
PJ_LP proj_hgrid_apply_internal(PJ_CONTEXT* ctx,
                       PJ_LP lp,
                       PJ_DIRECTION direction,
                       const ListOfHGrids& grids)
{
    PJ_LP out;

    out.lam = HUGE_VAL;
    out.phi = HUGE_VAL;

    const auto grid = findGrid(grids, lp);
    if( !grid ) {
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return out;
    }
    if( grid->isNullGrid() )
    {
        return lp;
    }

    int inverse = direction == PJ_FWD ? 0 : 1;
    out = nad_cvt(lp, inverse, grid);

    if (out.lam == HUGE_VAL || out.phi == HUGE_VAL)
        pj_ctx_set_errno(ctx, PJD_ERR_GRID_AREA);

    return out;
}

PJ_LP proj_hgrid_apply(PJ *P, const ListOfHGrids& grids, PJ_LP lp, PJ_DIRECTION direction) {
    return proj_hgrid_apply_internal(P->ctx, lp, direction, grids);
}


NS_PROJ_END

/************************************************************************/
/*                         pj_apply_gridshift()                         */
/*                                                                      */
/*      This is the externally callable interface - part of the         */
/*      public API - though it is not used internally any more and I    */
/*      doubt it is used by any other applications.  But we preserve    */
/*      it to honour our public api.                                    */
/************************************************************************/

int pj_apply_gridshift( projCtx ctx, const char *nadgrids, int inverse,
                        long point_count, int point_offset,
                        double *x, double *y, double * /*z */ )

{
    auto hgrids = NS_PROJ::getListOfGridSets(ctx, nadgrids);
    if( hgrids.empty() )
    {
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 1;
    }


    for( long i = 0; i < point_count; i++ )
    {
        PJ_LP   input;

        long io = i * point_offset;
        input.phi = y[io];
        input.lam = x[io];

        auto output = proj_hgrid_apply_internal(ctx, input, inverse ? PJ_INV : PJ_FWD, hgrids);

        if ( output.lam != HUGE_VAL )
        {
            y[io] = output.phi;
            x[io] = output.lam;
        }
        else
        {
            if( ctx->debug_level >= PJ_LOG_DEBUG_MAJOR )
            {
                pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                    "pj_apply_gridshift(): failed to find a grid shift table for\n"
                    "                      location (%.7fdW,%.7fdN)",
                    x[io] * RAD_TO_DEG,
                    y[io] * RAD_TO_DEG );
            }
        }
    }

    return 0;
}
