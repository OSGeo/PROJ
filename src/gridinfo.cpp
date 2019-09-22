/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Functions for handling individual PJ_GRIDINFO's.  Includes
 *           loaders for all formats but CTABLE (in nad_init.c).
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

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj_internal.h"

/************************************************************************/
/*                             swap_words()                             */
/*                                                                      */
/*      Convert the byte order of the given word(s) in place.           */
/************************************************************************/

static const int  byte_order_test = 1;
#define IS_LSB	(1 == ((const unsigned char *) (&byte_order_test))[0])

static void swap_words( unsigned char *data, int word_size, int word_count )

{
    int	word;

    for( word = 0; word < word_count; word++ )
    {
        int	i;

        for( i = 0; i < word_size/2; i++ )
        {
            unsigned char	t;

            t = data[i];
            data[i] = data[word_size-i-1];
            data[word_size-i-1] = t;
        }

        data += word_size;
    }
}

/************************************************************************/
/*                             to_double()                              */
/*                                                                      */
/*      Returns a double from the pointed data.                         */
/************************************************************************/

static double to_double( unsigned char* data )
{
    double d;
    memcpy(&d, data, sizeof(d));
    return d;
}

/************************************************************************/
/*                          pj_gridinfo_free()                          */
/************************************************************************/

void pj_gridinfo_free( projCtx ctx, PJ_GRIDINFO *gi )

{
    if( gi == nullptr )
        return;

    if( gi->child != nullptr )
    {
        PJ_GRIDINFO *child, *next;

        for( child = gi->child; child != nullptr; child=next)
        {
            next=child->next;
            pj_gridinfo_free( ctx, child );
        }
    }

    if( gi->ct != nullptr )
        nad_free( gi->ct );

    free( gi->gridname );
    if( gi->filename != nullptr )
        free( gi->filename );

    pj_dalloc( gi );
}

/************************************************************************/
/*                          pj_gridinfo_load()                          */
/*                                                                      */
/*      This function is intended to implement delayed loading of       */
/*      the data contents of a grid file.  The header and related       */
/*      stuff are loaded by pj_gridinfo_init().                         */
/************************************************************************/

int pj_gridinfo_load( projCtx_t* ctx, PJ_GRIDINFO *gi )

{
    struct CTABLE ct_tmp;

    if( gi == nullptr || gi->ct == nullptr )
        return 0;

    pj_acquire_lock();
    if( gi->ct->cvs != nullptr )
    {
        pj_release_lock();
        return 1;
    }

    memcpy(&ct_tmp, gi->ct, sizeof(struct CTABLE));

/* -------------------------------------------------------------------- */
/*      Original platform specific CTable format.                       */
/* -------------------------------------------------------------------- */
    if( strcmp(gi->format,"ctable") == 0 )
    {
        PAFile fid;
        int result;

        fid = pj_open_lib( ctx, gi->filename, "rb" );

        if( fid == nullptr )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        result = nad_ctable_load( ctx, &ct_tmp, (struct projFileAPI_t*)fid );

        pj_ctx_fclose( ctx, fid );

        gi->ct->cvs = ct_tmp.cvs;
        pj_release_lock();

        return result;
    }

/* -------------------------------------------------------------------- */
/*      CTable2 format.                                                 */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"ctable2") == 0 )
    {
        PAFile fid;
        int result;

        fid = pj_open_lib( ctx, gi->filename, "rb" );

        if( fid == nullptr )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        result = nad_ctable2_load( ctx, &ct_tmp, (struct projFileAPI_t*)fid );

        pj_ctx_fclose( ctx, fid );

        gi->ct->cvs = ct_tmp.cvs;

        pj_release_lock();
        return result;
    }

/* -------------------------------------------------------------------- */
/*      NTv1 format.                                                    */
/*      We process one line at a time.  Note that the array storage     */
/*      direction (e-w) is different in the NTv1 file and what          */
/*      the CTABLE is supposed to have.  The phi/lam are also           */
/*      reversed, and we have to be aware of byte swapping.             */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"ntv1") == 0 )
    {
        double	*row_buf;
        int	row;
        PAFile fid;

        fid = pj_open_lib( ctx, gi->filename, "rb" );

        if( fid == nullptr )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        pj_ctx_fseek( ctx, fid, gi->grid_offset, SEEK_SET );

        row_buf = (double *) pj_malloc(gi->ct->lim.lam * sizeof(double) * 2);
        ct_tmp.cvs = (FLP *) pj_malloc(gi->ct->lim.lam*gi->ct->lim.phi*sizeof(FLP));
        if( row_buf == nullptr || ct_tmp.cvs == nullptr )
        {
            pj_dalloc( row_buf );
            pj_dalloc( ct_tmp.cvs );
            pj_ctx_set_errno( ctx, ENOMEM );
            pj_release_lock();
            return 0;
        }

        for( row = 0; row < gi->ct->lim.phi; row++ )
        {
            int	    i;
            FLP     *cvs;
            double  *diff_seconds;

            if( pj_ctx_fread( ctx, row_buf,
                              sizeof(double), gi->ct->lim.lam * 2, fid )
                != (size_t)( 2 * gi->ct->lim.lam ) )
            {
                pj_dalloc( row_buf );
                pj_dalloc( ct_tmp.cvs );
                pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                pj_release_lock();
                return 0;
            }

            if( IS_LSB )
                swap_words( (unsigned char *) row_buf, 8, gi->ct->lim.lam*2 );

            /* convert seconds to radians */
            diff_seconds = row_buf;

            for( i = 0; i < gi->ct->lim.lam; i++ )
            {
                cvs = ct_tmp.cvs + (row) * gi->ct->lim.lam
                    + (gi->ct->lim.lam - i - 1);

                cvs->phi = (float) (*(diff_seconds++) * ((M_PI/180.0) / 3600.0));
                cvs->lam = (float) (*(diff_seconds++) * ((M_PI/180.0) / 3600.0));
            }
        }

        pj_dalloc( row_buf );

        pj_ctx_fclose( ctx, fid );

        gi->ct->cvs = ct_tmp.cvs;
        pj_release_lock();

        return 1;
    }

/* -------------------------------------------------------------------- */
/*      NTv2 format.                                                    */
/*      We process one line at a time.  Note that the array storage     */
/*      direction (e-w) is different in the NTv2 file and what          */
/*      the CTABLE is supposed to have.  The phi/lam are also           */
/*      reversed, and we have to be aware of byte swapping.             */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"ntv2") == 0 )
    {
        float	*row_buf;
        int	row;
        PAFile fid;

        pj_log( ctx, PJ_LOG_DEBUG_MINOR,
                "NTv2 - loading grid %s", gi->ct->id );

        fid = pj_open_lib( ctx, gi->filename, "rb" );

        if( fid == nullptr )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        pj_ctx_fseek( ctx, fid, gi->grid_offset, SEEK_SET );

        row_buf = (float *) pj_malloc(gi->ct->lim.lam * sizeof(float) * 4);
        ct_tmp.cvs = (FLP *) pj_malloc(gi->ct->lim.lam*gi->ct->lim.phi*sizeof(FLP));
        if( row_buf == nullptr || ct_tmp.cvs == nullptr )
        {
            pj_dalloc( row_buf );
            pj_dalloc( ct_tmp.cvs );
            pj_ctx_set_errno( ctx, ENOMEM );
            pj_release_lock();
            return 0;
        }

        for( row = 0; row < gi->ct->lim.phi; row++ )
        {
            int	    i;
            FLP     *cvs;
            float   *diff_seconds;

            if( pj_ctx_fread( ctx, row_buf, sizeof(float),
                              gi->ct->lim.lam*4, fid )
                != (size_t)( 4 * gi->ct->lim.lam ) )
            {
                pj_dalloc( row_buf );
                pj_dalloc( ct_tmp.cvs );
                pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
                pj_release_lock();
                return 0;
            }

            if( gi->must_swap )
                swap_words( (unsigned char *) row_buf, 4,
                            gi->ct->lim.lam*4 );

            /* convert seconds to radians */
            diff_seconds = row_buf;

            for( i = 0; i < gi->ct->lim.lam; i++ )
            {
                cvs = ct_tmp.cvs + (row) * gi->ct->lim.lam
                    + (gi->ct->lim.lam - i - 1);

                cvs->phi = (float) (*(diff_seconds++) * ((M_PI/180.0) / 3600.0));
                cvs->lam = (float) (*(diff_seconds++) * ((M_PI/180.0) / 3600.0));
                diff_seconds += 2; /* skip accuracy values */
            }
        }

        pj_dalloc( row_buf );

        pj_ctx_fclose( ctx, fid );

        gi->ct->cvs = ct_tmp.cvs;

        pj_release_lock();
        return 1;
    }

/* -------------------------------------------------------------------- */
/*      GTX format.                                                     */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"gtx") == 0 )
    {
        int   words = gi->ct->lim.lam * gi->ct->lim.phi;
        PAFile fid;

        fid = pj_open_lib( ctx, gi->filename, "rb" );

        if( fid == nullptr )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        pj_ctx_fseek( ctx, fid, gi->grid_offset, SEEK_SET );

        ct_tmp.cvs = (FLP *) pj_malloc(words*sizeof(float));
        if( ct_tmp.cvs == nullptr )
        {
            pj_ctx_set_errno( ctx, ENOMEM );
            pj_release_lock();
            return 0;
        }

        if( pj_ctx_fread( ctx, ct_tmp.cvs, sizeof(float), words, fid )
            != (size_t)words )
        {
            pj_dalloc( ct_tmp.cvs );
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            pj_release_lock();
            return 0;
        }

        if( IS_LSB )
            swap_words( (unsigned char *) ct_tmp.cvs, 4, words );

        pj_ctx_fclose( ctx, fid );
        gi->ct->cvs = ct_tmp.cvs;
        pj_release_lock();
        return 1;
    }

    else
    {
        pj_release_lock();
        return 0;
    }
}

/************************************************************************/
/*                        gridinfo_parent()                          */
/*                                                                      */
/*      Seek a parent grid file by name from a grid list                */
/************************************************************************/

static PJ_GRIDINFO* gridinfo_parent( PJ_GRIDINFO *gilist,
        const char *name, int length )
{
    while( gilist )
    {
        if( strncmp(gilist->ct->id,name,length) == 0 ) return gilist;
        if( gilist->child )
        {
            PJ_GRIDINFO *parent=gridinfo_parent( gilist->child, name, length );
            if( parent ) return parent;
        }
        gilist=gilist->next;
    }
    return gilist;
}

/************************************************************************/
/*                       pj_gridinfo_init_ntv2()                        */
/*                                                                      */
/*      Load a ntv2 (.gsb) file.                                        */
/************************************************************************/

static int pj_gridinfo_init_ntv2( projCtx ctx, PAFile fid, PJ_GRIDINFO *gilist )
{
    unsigned char header[11*16];
    int num_subfiles, subfile;
    int must_swap;

    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(pj_int32) == 4 );
    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(double) == 8 );

/* -------------------------------------------------------------------- */
/*      Read the overview header.                                       */
/* -------------------------------------------------------------------- */
    if( pj_ctx_fread( ctx, header, sizeof(header), 1, fid ) != 1 )
    {
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 0;
    }

    if( header[8] == 11 )
        must_swap = !IS_LSB;
    else
        must_swap = IS_LSB;

/* -------------------------------------------------------------------- */
/*      Byte swap interesting fields if needed.                         */
/* -------------------------------------------------------------------- */
    if( must_swap )
    {
        swap_words( header+8, 4, 1 );
        swap_words( header+8+16, 4, 1 );
        swap_words( header+8+32, 4, 1 );
        swap_words( header+8+7*16, 8, 1 );
        swap_words( header+8+8*16, 8, 1 );
        swap_words( header+8+9*16, 8, 1 );
        swap_words( header+8+10*16, 8, 1 );
    }

/* -------------------------------------------------------------------- */
/*      Get the subfile count out ... all we really use for now.        */
/* -------------------------------------------------------------------- */
    memcpy( &num_subfiles, header+8+32, 4 );

/* ==================================================================== */
/*      Step through the subfiles, creating a PJ_GRIDINFO for each.     */
/* ==================================================================== */
    for( subfile = 0; subfile < num_subfiles; subfile++ )
    {
        struct CTABLE *ct;
        PJ_LP ur;
        int gs_count;
        PJ_GRIDINFO *gi;

/* -------------------------------------------------------------------- */
/*      Read header.                                                    */
/* -------------------------------------------------------------------- */
        if( pj_ctx_fread( ctx, header, sizeof(header), 1, fid ) != 1 )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return 0;
        }

        if( strncmp((const char *) header,"SUB_NAME",8) != 0 )
        {
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return 0;
        }

/* -------------------------------------------------------------------- */
/*      Byte swap interesting fields if needed.                         */
/* -------------------------------------------------------------------- */
        if( must_swap )
        {
            swap_words( header+8+16*4, 8, 1 );
            swap_words( header+8+16*5, 8, 1 );
            swap_words( header+8+16*6, 8, 1 );
            swap_words( header+8+16*7, 8, 1 );
            swap_words( header+8+16*8, 8, 1 );
            swap_words( header+8+16*9, 8, 1 );
            swap_words( header+8+16*10, 4, 1 );
        }

/* -------------------------------------------------------------------- */
/*      Initialize a corresponding "ct" structure.                      */
/* -------------------------------------------------------------------- */
        ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
        if (!ct) {
            pj_ctx_set_errno(ctx, ENOMEM);
            return 0;
        }
        strncpy( ct->id, (const char *) header + 8, 8 );
        ct->id[8] = '\0';

        ct->ll.lam = - to_double(header+7*16+8); /* W_LONG */
        ct->ll.phi = to_double(header+4*16+8);   /* S_LAT */

        ur.lam = - to_double(header+6*16+8);     /* E_LONG */
        ur.phi = to_double(header+5*16+8);       /* N_LAT */

        ct->del.lam = to_double(header+9*16+8);
        ct->del.phi = to_double(header+8*16+8);

        ct->lim.lam = (pj_int32) (fabs(ur.lam-ct->ll.lam)/ct->del.lam + 0.5) + 1;
        ct->lim.phi = (pj_int32) (fabs(ur.phi-ct->ll.phi)/ct->del.phi + 0.5) + 1;

        pj_log( ctx, PJ_LOG_DEBUG_MINOR,
                "NTv2 %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)",
                ct->id,
                ct->lim.lam, ct->lim.phi,
                ct->ll.lam/3600.0, ct->ll.phi/3600.0,
                ur.lam/3600.0, ur.phi/3600.0 );

        ct->ll.lam *= DEG_TO_RAD/3600.0;
        ct->ll.phi *= DEG_TO_RAD/3600.0;
        ct->del.lam *= DEG_TO_RAD/3600.0;
        ct->del.phi *= DEG_TO_RAD/3600.0;

        memcpy( &gs_count, header + 8 + 16*10, 4 );
        if( gs_count != ct->lim.lam * ct->lim.phi )
        {
            pj_log( ctx, PJ_LOG_ERROR,
                    "GS_COUNT(%d) does not match expected cells (%dx%d=%d)",
                    gs_count, ct->lim.lam, ct->lim.phi,
                    ct->lim.lam * ct->lim.phi );
            pj_dalloc(ct);
            pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
            return 0;
        }

        ct->cvs = nullptr;

/* -------------------------------------------------------------------- */
/*      Create a new gridinfo for this if we aren't processing the      */
/*      1st subfile, and initialize our grid info.                      */
/* -------------------------------------------------------------------- */
        if( subfile == 0 )
            gi = gilist;
        else
        {
            gi = (PJ_GRIDINFO *) pj_calloc(1, sizeof(PJ_GRIDINFO));
            if (!gi) {
                pj_dalloc(ct);
                pj_gridinfo_free(ctx, gilist);
                pj_ctx_set_errno(ctx, ENOMEM);
                return 0;
            }

            gi->gridname = pj_strdup( gilist->gridname );
            gi->filename = pj_strdup( gilist->filename );
            if (!gi->gridname || !gi->filename) {
                pj_gridinfo_free(ctx, gi);
                pj_dalloc(ct);
                pj_gridinfo_free(ctx, gilist);
                pj_ctx_set_errno(ctx, ENOMEM);
                return 0;
            }
            gi->next = nullptr;
        }

        gi->must_swap = must_swap;
        gi->ct = ct;
        gi->format = "ntv2";
        gi->grid_offset = pj_ctx_ftell( ctx, fid );

/* -------------------------------------------------------------------- */
/*      Attach to the correct list or sublist.                          */
/* -------------------------------------------------------------------- */
        if( strncmp((const char *)header+24,"NONE",4) == 0 )
        {
            if( gi != gilist )
            {
                PJ_GRIDINFO *lnk;

                for( lnk = gilist; lnk->next != nullptr; lnk = lnk->next ) {}
                lnk->next = gi;
            }
        }

        else
        {
            PJ_GRIDINFO *lnk;
            PJ_GRIDINFO *gp = gridinfo_parent(gilist,
                                                 (const char*)header+24,8);

            if( gp == nullptr )
            {
                pj_log( ctx, PJ_LOG_ERROR,
                        "pj_gridinfo_init_ntv2(): "
                        "failed to find parent %8.8s for %s.",
                        (const char *) header+24, gi->ct->id );

                for( lnk = gilist; lnk->next != nullptr; lnk = lnk->next ) {}
                lnk->next = gi;
            }
            else
            {
                if( gp->child == nullptr )
                {
                    gp->child = gi;
                }
                else
                {
                    for( lnk = gp->child; lnk->next != nullptr; lnk = lnk->next ) {}
                    lnk->next = gi;
                }
            }
        }

/* -------------------------------------------------------------------- */
/*      Seek past the data.                                             */
/* -------------------------------------------------------------------- */
        pj_ctx_fseek( ctx, fid, gs_count * 16, SEEK_CUR );
    }

    return 1;
}

/************************************************************************/
/*                       pj_gridinfo_init_ntv1()                        */
/*                                                                      */
/*      Load an NTv1 style Canadian grid shift file.                    */
/************************************************************************/

static int pj_gridinfo_init_ntv1( projCtx ctx, PAFile fid, PJ_GRIDINFO *gi )

{
    unsigned char header[192]; /* 12 records of 16 bytes */
    struct CTABLE *ct;
    PJ_LP		ur;

    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(pj_int32) == 4 );
    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(double) == 8 );

/* -------------------------------------------------------------------- */
/*      Read the header.                                                */
/* -------------------------------------------------------------------- */
    if( pj_ctx_fread( ctx, header, sizeof(header), 1, fid ) != 1 )
    {
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Regularize fields of interest.                                  */
/* -------------------------------------------------------------------- */
    if( IS_LSB )
    {
        swap_words( header+8, 4, 1 );
        swap_words( header+24, 8, 1 );
        swap_words( header+40, 8, 1 );
        swap_words( header+56, 8, 1 );
        swap_words( header+72, 8, 1 );
        swap_words( header+88, 8, 1 );
        swap_words( header+104, 8, 1 );
    }

    if( *((int *) (header+8)) != 12 )
    {
        pj_log( ctx, PJ_LOG_ERROR,
                "NTv1 grid shift file has wrong record count, corrupt?" );
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Fill in CTABLE structure.                                       */
/* -------------------------------------------------------------------- */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    if (!ct) {
        pj_ctx_set_errno(ctx, ENOMEM);
        return 0;
    }
    strcpy( ct->id, "NTv1 Grid Shift File" );

    ct->ll.lam = - to_double(header+72);
    ct->ll.phi = to_double(header+24);
    ur.lam = - to_double(header+56);
    ur.phi = to_double(header+40);
    ct->del.lam = to_double(header+104);
    ct->del.phi = to_double(header+88);
    ct->lim.lam = (pj_int32) (fabs(ur.lam-ct->ll.lam)/ct->del.lam + 0.5) + 1;
    ct->lim.phi = (pj_int32) (fabs(ur.phi-ct->ll.phi)/ct->del.phi + 0.5) + 1;

    pj_log( ctx, PJ_LOG_DEBUG_MINOR,
            "NTv1 %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)",
            ct->lim.lam, ct->lim.phi,
            ct->ll.lam, ct->ll.phi, ur.lam, ur.phi );

    ct->ll.lam *= DEG_TO_RAD;
    ct->ll.phi *= DEG_TO_RAD;
    ct->del.lam *= DEG_TO_RAD;
    ct->del.phi *= DEG_TO_RAD;
    ct->cvs = nullptr;

    gi->ct = ct;
    gi->grid_offset = (long) sizeof(header);
    gi->format = "ntv1";

    return 1;
}

/************************************************************************/
/*                       pj_gridinfo_init_gtx()                         */
/*                                                                      */
/*      Load a NOAA .gtx vertical datum shift file.                     */
/************************************************************************/

static int pj_gridinfo_init_gtx( projCtx ctx, PAFile fid, PJ_GRIDINFO *gi )

{
    unsigned char header[40];
    struct CTABLE *ct;
    double      xorigin,yorigin,xstep,ystep;
    int         rows, columns;

    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(pj_int32) == 4 );
    /* cppcheck-suppress sizeofCalculation */
    STATIC_ASSERT( sizeof(double) == 8 );

/* -------------------------------------------------------------------- */
/*      Read the header.                                                */
/* -------------------------------------------------------------------- */
    if( pj_ctx_fread( ctx, header, sizeof(header), 1, fid ) != 1 )
    {
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Regularize fields of interest and extract.                      */
/* -------------------------------------------------------------------- */
    if( IS_LSB )
    {
        swap_words( header+0, 8, 4 );
        swap_words( header+32, 4, 2 );
    }

    memcpy( &yorigin, header+0, 8 );
    memcpy( &xorigin, header+8, 8 );
    memcpy( &ystep, header+16, 8 );
    memcpy( &xstep, header+24, 8 );

    memcpy( &rows, header+32, 4 );
    memcpy( &columns, header+36, 4 );

    if( xorigin < -360 || xorigin > 360
        || yorigin < -90 || yorigin > 90 )
    {
        pj_log( ctx, PJ_LOG_ERROR,
                "gtx file header has invalid extents, corrupt?");
        pj_ctx_set_errno( ctx, PJD_ERR_FAILED_TO_LOAD_GRID );
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Fill in CTABLE structure.                                       */
/* -------------------------------------------------------------------- */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    if (!ct) {
        pj_ctx_set_errno(ctx, ENOMEM);
        return 0;
    }
    strcpy( ct->id, "GTX Vertical Grid Shift File" );

    ct->ll.lam = xorigin;
    ct->ll.phi = yorigin;
    ct->del.lam = xstep;
    ct->del.phi = ystep;
    ct->lim.lam = columns;
    ct->lim.phi = rows;

    /* some GTX files come in 0-360 and we shift them back into the
       expected -180 to 180 range if possible.  This does not solve
       problems with grids spanning the dateline. */
    if( ct->ll.lam >= 180.0 )
        ct->ll.lam -= 360.0;

    if( ct->ll.lam >= 0.0 && ct->ll.lam + ct->del.lam * ct->lim.lam > 180.0 )
    {
        pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                "This GTX spans the dateline!  This will cause problems." );
    }

    pj_log( ctx, PJ_LOG_DEBUG_MINOR,
            "GTX %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)",
            ct->lim.lam, ct->lim.phi,
            ct->ll.lam, ct->ll.phi,
            ct->ll.lam + (columns-1)*xstep, ct->ll.phi + (rows-1)*ystep);

    ct->ll.lam *= DEG_TO_RAD;
    ct->ll.phi *= DEG_TO_RAD;
    ct->del.lam *= DEG_TO_RAD;
    ct->del.phi *= DEG_TO_RAD;
    ct->cvs = nullptr;

    gi->ct = ct;
    gi->grid_offset = 40;
    gi->format = "gtx";

    return 1;
}

/************************************************************************/
/*                          pj_gridinfo_init()                          */
/*                                                                      */
/*      Open and parse header details from a datum gridshift file       */
/*      returning a list of PJ_GRIDINFOs for the grids in that          */
/*      file.  This superceeds use of nad_init() for modern             */
/*      applications.                                                   */
/************************************************************************/

PJ_GRIDINFO *pj_gridinfo_init( projCtx ctx, const char *gridname )

{
    PJ_GRIDINFO *gilist;
    PAFile 	fp;
    char	header[160];
    size_t      header_size = 0;

    errno = pj_errno = 0;
    ctx->last_errno = 0;

/* -------------------------------------------------------------------- */
/*      Initialize a GRIDINFO with stub info we would use if it         */
/*      cannot be loaded.                                               */
/* -------------------------------------------------------------------- */
    gilist = (PJ_GRIDINFO *) pj_calloc(1, sizeof(PJ_GRIDINFO));
    if (!gilist) {
        pj_ctx_set_errno(ctx, ENOMEM);
        return nullptr;
    }

    gilist->gridname = pj_strdup( gridname );
    if (!gilist->gridname) {
        pj_dalloc(gilist);
        pj_ctx_set_errno(ctx, ENOMEM);
        return nullptr;
    }
    gilist->filename = nullptr;
    gilist->format = "missing";
    gilist->grid_offset = 0;
    gilist->ct = nullptr;
    gilist->next = nullptr;

/* -------------------------------------------------------------------- */
/*      Open the file using the usual search rules.                     */
/* -------------------------------------------------------------------- */
    if (!(fp = pj_open_lib(ctx, gridname, "rb"))) {
        ctx->last_errno = 0; /* don't treat as a persistent error */
        return gilist;
    }

    gilist->filename = pj_strdup(gridname);
    if (!gilist->filename) {
        pj_dalloc(gilist->gridname);
        pj_dalloc(gilist);
        pj_ctx_set_errno(ctx, ENOMEM);
        return nullptr;
    }

/* -------------------------------------------------------------------- */
/*      Load a header, to determine the file type.                      */
/* -------------------------------------------------------------------- */
    if( (header_size = pj_ctx_fread( ctx, header, 1,
                                     sizeof(header), fp ) ) != sizeof(header) )
    {
        /* some files may be smaller that sizeof(header), eg 160, so */
        ctx->last_errno = 0; /* don't treat as a persistent error */
        pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                "pj_gridinfo_init: short header read of %d bytes",
                (int)header_size );
    }

    pj_ctx_fseek( ctx, fp, SEEK_SET, 0 );

/* -------------------------------------------------------------------- */
/*      Determine file type.                                            */
/* -------------------------------------------------------------------- */
    if( header_size >= 144 + 16
        && strncmp(header + 0, "HEADER", 6) == 0
        && strncmp(header + 96, "W GRID", 6) == 0
        && strncmp(header + 144, "TO      NAD83   ", 16) == 0 )
    {
        pj_gridinfo_init_ntv1( ctx, fp, gilist );
    }

    else if( header_size >= 48 + 7
             && strncmp(header + 0, "NUM_OREC", 8) == 0
             && strncmp(header + 48, "GS_TYPE", 7) == 0 )
    {
        pj_gridinfo_init_ntv2( ctx, fp, gilist );
    }

    else if( strlen(gridname) > 4
             && (strcmp(gridname+strlen(gridname)-3,"gtx") == 0
                 || strcmp(gridname+strlen(gridname)-3,"GTX") == 0) )
    {
        pj_gridinfo_init_gtx( ctx, fp, gilist );
    }

    else if( header_size >= 9 && strncmp(header + 0,"CTABLE V2",9) == 0 )
    {
        struct CTABLE *ct = nad_ctable2_init( ctx, (struct projFileAPI_t*)fp );

        gilist->format = "ctable2";
        gilist->ct = ct;

        if (ct == nullptr)
        {
            pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                    "CTABLE V2 ct is NULL.");
        }
        else
        {
            pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                    "Ctable2 %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)",
                    ct->id,
                    ct->lim.lam, ct->lim.phi,
                    ct->ll.lam * RAD_TO_DEG, ct->ll.phi * RAD_TO_DEG,
                    (ct->ll.lam + (ct->lim.lam-1)*ct->del.lam) * RAD_TO_DEG,
                    (ct->ll.phi + (ct->lim.phi-1)*ct->del.phi) * RAD_TO_DEG );
        }
    }

    else
    {
        struct CTABLE *ct = nad_ctable_init( ctx, (struct projFileAPI_t*)fp );
        if (ct == nullptr)
        {
            pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                    "CTABLE ct is NULL.");
        } else
        {
            gilist->format = "ctable";
            gilist->ct = ct;

            pj_log( ctx, PJ_LOG_DEBUG_MAJOR,
                    "Ctable %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)",
                    ct->id,
                    ct->lim.lam, ct->lim.phi,
                    ct->ll.lam * RAD_TO_DEG, ct->ll.phi * RAD_TO_DEG,
                    (ct->ll.lam + (ct->lim.lam-1)*ct->del.lam) * RAD_TO_DEG,
                    (ct->ll.phi + (ct->lim.phi-1)*ct->del.phi) * RAD_TO_DEG );
        }
    }

    pj_ctx_fclose(ctx, fp);

    return gilist;
}
