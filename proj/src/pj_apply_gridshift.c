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
 ******************************************************************************
 *
 * $Log$
 * Revision 1.5  2003/03/15 06:02:02  warmerda
 * preliminary NTv2 support, major restructure of datum shifting
 *
 * Revision 1.4  2002/07/08 02:32:05  warmerda
 * ensure clean C++ builds
 *
 * Revision 1.3  2002/04/30 16:27:27  warmerda
 * improve debug output
 *
 * Revision 1.2  2001/03/15 16:57:55  warmerda
 * fixed intermittent problem in pj_load_nadgrids()
 *
 * Revision 1.1  2000/07/06 23:32:27  warmerda
 * New
 *
 */

#define PJ_LIB__

#include <projects.h>
#include <string.h>
#include <math.h>

/************************************************************************/
/*                         pj_apply_gridshift()                         */
/************************************************************************/

int pj_apply_gridshift( const char *nadgrids, int inverse, 
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    int grid_count = 0;
    PJ_GRIDINFO   **tables = pj_gridlist_from_nadgrids( nadgrids, &grid_count);
    int  i;

    if( tables == NULL || grid_count == 0 )
        return pj_errno;

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;
        LP   input, output;
        int  itable;

        input.phi = y[io];
        input.lam = x[io];

        /* keep trying till we find a table that works */
        for( itable = 0; itable < grid_count; itable++ )
        {
            struct CTABLE *ct = tables[itable]->ct;
            
            /* skip tables that don't match our point at all.  */
            if( ct->ll.phi > input.phi || ct->ll.lam > input.lam
                || ct->ll.phi + ct->lim.phi * ct->del.phi < input.phi
                || ct->ll.lam + ct->lim.lam * ct->del.lam < input.lam )
                continue;

            /* load the grid shift info if we don't have it. */
            if( tables[itable]->ct->cvs == NULL 
                && !pj_gridinfo_load( tables[itable] ) )
            {
                pj_errno = -38;
                return pj_errno;
            }
            
            output = nad_cvt( input, inverse, tables[itable]->ct );
            if( output.lam != HUGE_VAL )
                break;
        }

        if( output.lam == HUGE_VAL )
        {
            if( getenv( "PROJ_DEBUG" ) != NULL )
            {
                fprintf( stderr, 
                         "pj_apply_gridshift(): failed to find a grid shift table for\n"
                         "                      location (%.7fdW,%.7fdN)\n",
                         x[io] * RAD_TO_DEG, 
                         y[io] * RAD_TO_DEG );
                fprintf( stderr, 
                         "   tried: %s\n", nadgrids );
            }
        
            pj_errno = -38;
            return pj_errno;
        }
        else
        {
            y[io] = output.phi;
            x[io] = output.lam;
        }
    }

    return 0;
}

