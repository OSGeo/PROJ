/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Code to read a grid catalog from a .cvs file.
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

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "projects.h"

static int gc_readentry(projCtx ctx, PAFile fid, PJ_GridCatalogEntry *entry);

/************************************************************************/
/*                         pj_gc_readcatalog()                          */
/*                                                                      */
/*      Read a grid catalog from a .csv file.                           */
/************************************************************************/

PJ_GridCatalog *pj_gc_readcatalog( projCtx ctx, const char *catalog_name )
{
    PAFile fid;
    PJ_GridCatalog *catalog;
    int entry_max;
    char line[302];

    fid = pj_open_lib( ctx, catalog_name, "r" );
    if (fid == NULL) 
        return NULL;

    /* discard title line */
    pj_ctx_fgets(ctx, line, sizeof(line)-1, fid);

    catalog = (PJ_GridCatalog *) calloc(1,sizeof(PJ_GridCatalog));
    if( !catalog )
    {
        pj_ctx_set_errno(ctx, ENOMEM);
        pj_ctx_fclose(ctx, fid);
        return NULL;
    }
    
    catalog->catalog_name = pj_strdup(catalog_name);
    if (!catalog->catalog_name) {
        pj_ctx_set_errno(ctx, ENOMEM);
        free(catalog);
        pj_ctx_fclose(ctx, fid);
        return NULL;
    }
    
    entry_max = 10;
    catalog->entries = (PJ_GridCatalogEntry *) 
        malloc(entry_max * sizeof(PJ_GridCatalogEntry));
    if (!catalog->entries) {
        pj_ctx_set_errno(ctx, ENOMEM);
        free(catalog->catalog_name);
        free(catalog);
        pj_ctx_fclose(ctx, fid);
        return NULL;
    }
    
    while( gc_readentry( ctx, fid, 
                            catalog->entries+catalog->entry_count) == 0)
    {
        catalog->entry_count++;
        
        if( catalog->entry_count == entry_max ) 
        {
            PJ_GridCatalogEntry* new_entries;
            entry_max = entry_max * 2;
            new_entries = (PJ_GridCatalogEntry *) 
                realloc(catalog->entries, 
                        entry_max * sizeof(PJ_GridCatalogEntry));
            if (new_entries == NULL )
            {
                int i;
                for( i = 0; i < catalog->entry_count; i++ )
                    free( catalog->entries[i].definition );
                free( catalog->entries );
                free( catalog->catalog_name );
                free( catalog );
                pj_ctx_fclose(ctx, fid);
                return NULL;
            }
            catalog->entries = new_entries;
        }
    }

    pj_ctx_fclose(ctx, fid);

    return catalog;
}

/************************************************************************/
/*                        gc_read_csv_line()                         */
/*                                                                      */
/*      Simple csv line splitter with fixed maximum line size and       */
/*      token count.                                                    */
/************************************************************************/

static int gc_read_csv_line( projCtx ctx, PAFile fid, 
                                char **tokens, int max_tokens ) 
{
    char line[302];
   
    while( pj_ctx_fgets(ctx, line, sizeof(line)-1, fid) != NULL )
    {
        char *next = line;
        int token_count = 0;
        
        while( isspace(*next) ) 
            next++;
        
        /* skip blank and comment lines */
        if( next[0] == '#' || next[0] == '\0' )
            continue;
        
        while( token_count < max_tokens && *next != '\0' ) 
        {
            const char *start = next;
            char* token;
            
            while( *next != '\0' && *next != ',' ) 
                next++;
            
            if( *next == ',' )
            {
                *next = '\0';
                next++;
            }
            
            token = pj_strdup(start);
            if (!token) {
                while (token_count > 0)
                    free(tokens[--token_count]);
                pj_ctx_set_errno(ctx, ENOMEM);
                return 0;
            }
            tokens[token_count++] = token;
        }

        return token_count;
    }
    
    return 0; 
}

/************************************************************************/
/*                          pj_gc_parsedate()                           */
/*                                                                      */
/*      Parse a date into a floating point year value.  Acceptable      */
/*      values are "yyyy.fraction" and "yyyy-mm-dd".  Anything else     */
/*      returns 0.0.                                                    */
/************************************************************************/

double pj_gc_parsedate( projCtx ctx, const char *date_string )
{
    (void) ctx;

    if( strlen(date_string) == 10 
        && date_string[4] == '-' && date_string[7] == '-' ) 
    {
        int year = atoi(date_string);
        int month = atoi(date_string+5);
        int day = atoi(date_string+8);

        /* simplified calculation so we don't need to know all about months */
        return year + ((month-1) * 31 + (day-1)) / 372.0;
    }
    else 
    {
        return pj_atof(date_string);
    }
}


/************************************************************************/
/*                          gc_readentry()                           */
/*                                                                      */
/*      Read one catalog entry from the file                            */
/*                                                                      */
/*      Format:                                                         */
/*        gridname,ll_long,ll_lat,ur_long,ur_lat,priority,date          */
/************************************************************************/

static int gc_readentry(projCtx ctx, PAFile fid, PJ_GridCatalogEntry *entry) 
{
#define MAX_TOKENS 30
    char *tokens[MAX_TOKENS];
    int token_count, i;
    int error = 0;

    memset( entry, 0, sizeof(PJ_GridCatalogEntry) );
    
    token_count = gc_read_csv_line( ctx, fid, tokens, MAX_TOKENS );
    if( token_count < 5 )
    {
        error = 1; /* TODO: need real error codes */
        if( token_count != 0 )
            pj_log( ctx, PJ_LOG_ERROR, "Short line in grid catalog." );
    }
    else
    {
        entry->definition = tokens[0];
        tokens[0] = NULL;   /* We take ownership of tokens[0] */
        entry->region.ll_long = dmstor_ctx( ctx, tokens[1], NULL );
        entry->region.ll_lat = dmstor_ctx( ctx, tokens[2], NULL );
        entry->region.ur_long = dmstor_ctx( ctx, tokens[3], NULL );
        entry->region.ur_lat = dmstor_ctx( ctx, tokens[4], NULL );
        if( token_count > 5 )
            entry->priority = atoi( tokens[5] ); /* defaults to zero */
        if( token_count > 6 )
            entry->date = pj_gc_parsedate( ctx, tokens[6] );
    }

    for( i = 0; i < token_count; i++ )
        free( tokens[i] );

    return error;
}



