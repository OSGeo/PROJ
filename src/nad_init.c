/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Load datum shift files into memory.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
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
 * Revision 1.3  2001/04/05 19:31:54  warmerda
 * substantially reorganized and added NTv1 support
 *
 */

#define PJ_LIB__

#include <projects.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

static int  byte_order_test = 1;
#define IS_LSB	(((unsigned char *) (&byte_order_test))[0] == 1)

/************************************************************************/
/*                            local_order()                             */
/*                                                                      */
/*      Convert the given words into local order in place.              */
/************************************************************************/

static void local_order( unsigned char *data, int word_size, int word_count )

{
    /* We only need to do work on LSB machines.  Perhaps we should 
       convert the data files into LSB order to cut workload! */

    if( IS_LSB )
    {
        int	word;

        for( word = 0; word < word_count; word++ )
        {
            int	i;

            for( i = 0; i < word_size/2; i++ )
            {
                int	t;

                t = data[i];
                data[i] = data[word_size-i-1];
                data[word_size-i-1] = t;
            }

            data += word_size;
        }
    }
}

/************************************************************************/
/*                           nad_load_ntv1()                            */
/*                                                                      */
/*      Load an NTv1 style Canadian grid shift file.                    */
/************************************************************************/

static struct CTABLE *nad_load_ntv1( FILE * fid )

{
    char	header[176];
    struct CTABLE *ct;
    LP		ur;
    double	*row_buf;
    int		row;
    
    assert( sizeof(int) == 4 );
    assert( sizeof(double) == 8 );
    if( sizeof(int) != 4 || sizeof(double) != 8 )
    {
        fprintf( stderr, 
                 "basic types of inappropraiate size in nad_load_ntv1()\n" );
        pj_errno = -38;
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Read the header.                                                */
/* -------------------------------------------------------------------- */
    if( fread( header, sizeof(header), 1, fid ) != 1 )
    {
        fclose( fid );
        pj_errno = -38;
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Regularize fields of interest.                                  */
/* -------------------------------------------------------------------- */
    local_order( header+8, 4, 1 );
    local_order( header+24, 8, 1 );
    local_order( header+40, 8, 1 );
    local_order( header+56, 8, 1 );
    local_order( header+72, 8, 1 );
    local_order( header+88, 8, 1 );
    local_order( header+104, 8, 1 );

    if( *((int *) (header+8)) != 12 )
    {
        pj_errno = -38;
        emess("NTv1 grid shift file has wrong record count, is it corrupt?");
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Fill in CTABLE structure.                                       */
/* -------------------------------------------------------------------- */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    strcpy( ct->id, "NTv1 Grid Shift File" );

    ct->ll.lam = - *((double *) (header+72));
    ct->ll.phi = *((double *) (header+24));
    ur.lam = - *((double *) (header+56));
    ur.phi = *((double *) (header+40));
    ct->del.lam = *((double *) (header+104));
    ct->del.phi = *((double *) (header+88));
    ct->lim.lam = (int) (fabs(ur.lam-ct->ll.lam)/ct->del.lam + 0.5) + 1;
    ct->lim.phi = (int) (fabs(ur.phi-ct->ll.phi)/ct->del.phi + 0.5) + 1;

    ct->ll.lam *= DEG_TO_RAD;
    ct->ll.phi *= DEG_TO_RAD;
    ct->del.lam *= DEG_TO_RAD;
    ct->del.phi *= DEG_TO_RAD;

/* -------------------------------------------------------------------- */
/*      Fill the data array.                                            */
/*                                                                      */
/*      We process one line at a time.  Note that the array storage     */
/*      direction (e-w) is different in the NTv1 file and what          */
/*      the CTABLE is supposed to have.  The phi/lam are also           */
/*      reversed, and we have to be aware of byte swapping.             */
/* -------------------------------------------------------------------- */
    row_buf = (double *) pj_malloc(ct->lim.lam * sizeof(double) * 2);
    ct->cvs = (FLP *) pj_malloc(ct->lim.lam*ct->lim.phi*sizeof(FLP));
    if( row_buf == NULL || ct->cvs == NULL )
        return NULL;

    for( row = 0; row < ct->lim.phi; row++ )
    {
        int	i;
        FLP     *cvs;
        double	*diff_seconds;

        if( fread( row_buf, sizeof(double), ct->lim.lam * 2, fid ) 
            != 2 * ct->lim.lam )
        {
            pj_dalloc( row_buf );
            pj_dalloc( ct->cvs );
            pj_errno = -38;
            return NULL;
        }

        local_order( (unsigned char *) row_buf, 8, ct->lim.lam * 2 );

        /* convert seconds to radians */
        diff_seconds = row_buf;

        for( i = 0; i < ct->lim.lam; i++ )
        {
            cvs = ct->cvs + (row) * ct->lim.lam
                + (ct->lim.lam - i - 1);

            cvs->phi = *(diff_seconds++) * ((PI/180.0) / 3600.0);
            cvs->lam = *(diff_seconds++) * ((PI/180.0) / 3600.0);
        }
    }

    pj_dalloc( row_buf );

    return ct;
}

/************************************************************************/
/*                          nad_load_ctable()                           */
/*                                                                      */
/*      Load a datum shift file already in "CTABLE" format.             */
/************************************************************************/

static struct CTABLE *nad_load_ctable( FILE * fid )
{
    struct CTABLE *ct;
    int		a_size;

    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    if( ct == NULL 
        || fread( ct, sizeof(struct CTABLE), 1, fid ) != 1 )
    {
        pj_errno = -38;
        return NULL;
    }

    if( ct->lim.lam < 1 || ct->lim.lam > 100000 
        || ct->lim.phi < 1 || ct->lim.phi > 100000 )
    {
        pj_errno = -38;
        return NULL;
    }
    
    a_size = ct->lim.lam * ct->lim.phi;
    ct->cvs = (FLP *) pj_malloc(sizeof(FLP) * a_size);
    if( ct->cvs == NULL 
        || fread(ct->cvs, sizeof(FLP), a_size, fid) != a_size )
    {
        nad_free( ct );
        pj_errno = -38;
        return NULL;
    }

    return ct;
}

/************************************************************************/
/*                              nad_init()                              */
/*                                                                      */
/*      Read a datum shift file in any of the supported binary formats. */
/************************************************************************/

struct CTABLE *nad_init(char *name) 
{
    char 	fname[MAX_PATH_FILENAME+1];
    struct CTABLE *ct;
    FILE 	*fid;
    char	header[512];

    errno = pj_errno = 0;

/* -------------------------------------------------------------------- */
/*      Open the file using the usual search rules.                     */
/* -------------------------------------------------------------------- */
    strcpy(fname, name);
    if (!(fid = pj_open_lib(fname, "rb"))) {
        pj_errno = errno;
        return 0;
    }
    
/* -------------------------------------------------------------------- */
/*      Load a header, to determine the file type.                      */
/* -------------------------------------------------------------------- */
    if( fread( header, sizeof(header), 1, fid ) != 1 )
    {
        fclose( fid );
        pj_errno = -38;
        return 0;
    }

    fseek( fid, SEEK_SET, 0 );

/* -------------------------------------------------------------------- */
/*      Determine file type.                                            */
/* -------------------------------------------------------------------- */
    if( strncmp(header + 0, "HEADER", 6) == 0 
        && strncmp(header + 96, "W GRID", 6) == 0 
        && strncmp(header + 144, "TO      NAD83   ", 16) == 0 )
    {
        ct = nad_load_ntv1( fid );
    }
    
    else
    {
        ct = nad_load_ctable( fid );
    }

    fclose(fid);
    return ct;
}

/************************************************************************/
/*                              nad_free()                              */
/*                                                                      */
/*      Free a CTABLE grid shift structure produced by nad_init().      */
/************************************************************************/

void nad_free(struct CTABLE *ct) 
{
    if (ct) {
        pj_dalloc(ct->cvs);
        pj_dalloc(ct);
    }
}
