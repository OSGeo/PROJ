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
 *****************************************************************************/

#define PJ_LIB__

#include <projects.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef _WIN32_WCE
/* assert.h includes all Windows API headers and causes 'LP' name clash.
 * Here assert we disable assert() for Windows CE.
 * TODO - mloskot: re-implement porting friendly assert
 */
# define assert(exp)	((void)0)
#else
# include <assert.h>
#endif /* _WIN32_WCE */

/************************************************************************/
/*                          nad_ctable_load()                           */
/*                                                                      */
/*      Load the data portion of a ctable formatted grid.               */
/************************************************************************/

int nad_ctable_load( struct CTABLE *ct, FILE *fid )

{
    int  a_size;

    fseek( fid, sizeof(struct CTABLE), SEEK_SET );

    /* read all the actual shift values */
    a_size = ct->lim.lam * ct->lim.phi;
    ct->cvs = (FLP *) pj_malloc(sizeof(FLP) * a_size);
    if( ct->cvs == NULL 
        || fread(ct->cvs, sizeof(FLP), a_size, fid) != a_size )
    {
        pj_dalloc( ct->cvs );
        ct->cvs = NULL;

        if( getenv("PROJ_DEBUG") != NULL )
        {
            fprintf( stderr, 
            "ctable loading failed on fread() - binary incompatible?\n" );
        }

        pj_errno = -38;
        return 0;
    }

    return 1;
} 

/************************************************************************/
/*                          nad_ctable_init()                           */
/*                                                                      */
/*      Read the header portion of a "ctable" format grid.              */
/************************************************************************/

struct CTABLE *nad_ctable_init( FILE * fid )
{
    struct CTABLE *ct;
    int		id_end;

    /* read the table header */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    if( ct == NULL 
        || fread( ct, sizeof(struct CTABLE), 1, fid ) != 1 )
    {
        pj_errno = -38;
        return NULL;
    }

    /* do some minimal validation to ensure the structure isn't corrupt */
    if( ct->lim.lam < 1 || ct->lim.lam > 100000 
        || ct->lim.phi < 1 || ct->lim.phi > 100000 )
    {
        pj_errno = -38;
        return NULL;
    }
    
    /* trim white space and newlines off id */
    for( id_end = strlen(ct->id)-1; id_end > 0; id_end-- )
    {
        if( ct->id[id_end] == '\n' || ct->id[id_end] == ' ' )
            ct->id[id_end] = '\0';
        else
            break;
    }

    ct->cvs = NULL;

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
    
    ct = nad_ctable_init( fid );
    if( ct != NULL )
    {
        if( !nad_ctable_load( ct, fid ) )
        {
            nad_free( ct );
            ct = NULL;
        }
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
        if( ct->cvs != NULL )
            pj_dalloc(ct->cvs);

        pj_dalloc(ct);
    }
}
