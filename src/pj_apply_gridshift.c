/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Apply datum shifts based on grid shift files (normally NAD27 to
 *           NAD83 or the reverse).  This module is responsible for keeping
 *           a list of loaded grids, and calling with each one that is 
 *           allowed for a given datum (expressed as the nadgrids= parameter).
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
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

#define GRID_MAX 100

/* used only by pj_get_grid() and pj_deallocate_grids() */
static int grid_count = 0;
static char **grid_names = NULL;
static struct CTABLE **grid_list = NULL;

/* used only by pj_load_nadgrids() and pj_deallocate_grids() */
static struct CTABLE **last_nadgrids_list = NULL;
static char          *last_nadgrids = NULL;

/************************************************************************/
/*                        pj_deallocate_grids()                         */
/*                                                                      */
/*      Deallocate all loaded grids.                                    */
/************************************************************************/

void pj_deallocate_grids()

{
    if( grid_count > 0 )
    {
        int        i;

        for( i = 0; i < grid_count; i++ )
        {
            if( grid_list[i] != NULL )
                nad_free( grid_list[i] );
            pj_dalloc( grid_names[i] );
        }
         
        pj_dalloc( grid_names );
        pj_dalloc( grid_list );

        grid_names = NULL;
        grid_list = NULL;

        grid_count = 0;
    }

    if( last_nadgrids != NULL )
    {
        pj_dalloc( last_nadgrids );
        last_nadgrids = NULL;

        pj_dalloc( last_nadgrids_list );
        last_nadgrids_list = NULL;
    }
}

/************************************************************************/
/*                            pj_get_grid()                             */
/*                                                                      */
/*      Find the requested grid in the list, or if not present, try     */
/*      and load it.  On failure returns NULL and sets pj_errno.        */
/************************************************************************/

static struct CTABLE *pj_get_grid( const char *name )

{
    int   i;

/* -------------------------------------------------------------------- */
/*      First look in the existing list.                                */
/* -------------------------------------------------------------------- */
    for( i = 0; i < grid_count; i++ )
    {
        if( strcmp( grid_names[i], name ) == 0 )
        {
            if( grid_list[i] == NULL )
                pj_errno = -38;

            return grid_list[i];
        }
    }

/* -------------------------------------------------------------------- */
/*      Add entry for this file in the grid list.                       */
/* -------------------------------------------------------------------- */
    if( grid_count == 0 )
    {
        grid_names = pj_malloc(sizeof(char *) * GRID_MAX);
        memset( grid_names, 0, sizeof(char *) * GRID_MAX );
        grid_list = pj_malloc(sizeof(struct CTABLE *) * GRID_MAX );
        memset( grid_list, 0, sizeof(struct CTABLE *) * GRID_MAX );
    }
    else if( grid_count >= GRID_MAX )
    {
        pj_errno = -38;
        return NULL;
    }

    grid_count++;

    grid_names[grid_count-1] = (char *) pj_malloc(strlen(name)+1);
    strcpy( grid_names[grid_count-1], name );

/* -------------------------------------------------------------------- */
/*      Read the file.                                                  */
/* -------------------------------------------------------------------- */
    grid_list[grid_count-1] = nad_init( (char *) name );

    return grid_list[grid_count-1];
}

/************************************************************************/
/*                          pj_load_nadgrids()                          */
/*                                                                      */
/*      This functions loads the list of grids corresponding to a       */
/*      particular nadgrids string into a list, and returns it.  The    */
/*      list is kept around till a request is made with a different     */
/*      string in order to cut down on the string parsing cost, and     */
/*      the cost of building the list of tables each time.              */
/************************************************************************/

static struct CTABLE **pj_load_nadgrids( const char *nadgrids )

{
    int     nadgrids_count = 0;
    const char *s;

    pj_errno = 0;

    if( last_nadgrids != NULL 
        && strcmp(nadgrids,last_nadgrids) == 0 )
        return last_nadgrids_list;

/* -------------------------------------------------------------------- */
/*      Free old one, if any, and make space for new list.              */
/* -------------------------------------------------------------------- */
    if( last_nadgrids != NULL )
    {
        pj_dalloc(last_nadgrids);
    }
    
    last_nadgrids = (char *) pj_malloc(strlen(nadgrids)+1);
    strcpy( last_nadgrids, nadgrids );

    if( last_nadgrids_list == NULL )
        last_nadgrids_list = (struct CTABLE **) 
            pj_malloc(sizeof(struct CTABLE *) * GRID_MAX);

/* -------------------------------------------------------------------- */
/*      Loop processing names out of nadgrids one at a time.            */
/* -------------------------------------------------------------------- */
    for( s = nadgrids; *s != '\0'; )
    {
        int   end_char;
        char  name[128];

        for( end_char = 0; 
             s[end_char] != '\0' && s[end_char] != ','; 
             end_char++ ) {}

        if( end_char > sizeof(name) )
        {
            pj_errno = -38;
            return NULL;
        }
        
        strncpy( name, s, end_char );
        name[end_char] = '\0';

        s += end_char;
        if( *s == ',' )
            s++;


        last_nadgrids_list[nadgrids_count] = pj_get_grid( name );
        if( last_nadgrids_list[nadgrids_count] == NULL )
        {
            pj_errno = -38;
            return NULL;
        }

        nadgrids_count++;
    }

    last_nadgrids_list[nadgrids_count] = NULL;

    return last_nadgrids_list;
}

/************************************************************************/
/*                         pj_apply_gridshift()                         */
/************************************************************************/

int pj_apply_gridshift( const char *nadgrids, int inverse, 
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    struct CTABLE   **tables = pj_load_nadgrids( nadgrids );
    int  i;

    if( tables == NULL )
        return pj_errno;

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;
        LP   input, output;
        int  itable;

        input.phi = y[io];
        input.lam = x[io];

        /* keep trying till we find a table that works */
        for( itable = 0; tables[itable] != NULL; itable++ )
        {
            output = nad_cvt( input, inverse, tables[itable] );
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

