/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Memory management for proj.4.
 *           This version includes an implementation of generic destructors,
 *           for memory deallocation for the large majority of PJ-objects
 *           that do not allocate anything else than the PJ-object itself,
 *           and its associated opaque object - i.e. no additional malloc'ed
 *           memory inside the opaque object.
 *
 * Author:   Gerald I. Evenden (Original proj.4 author),
 *           Frank Warmerdam   (2000)  pj_malloc?
 *           Thomas Knudsen    (2016) - freeup/dealloc parts
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 * Copyright (c) 2016, Thomas Knudsen / SDFE
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

/* allocate and deallocate memory */
/* These routines are used so that applications can readily replace
** projection system memory allocation/deallocation call with custom
** application procedures.  */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "proj.h"
#include "projects.h"

/**********************************************************************/
void *pj_malloc(size_t size) {
/***********************************************************************
Currently, pj_malloc is a hack to solve an errno problem.
The problem is described in more details at
https://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=86420.
It seems, that pj_init and similar functions incorrectly
(under debian/glibs-2.3.2) assume that pj_malloc resets
errno after success. pj_malloc tries to mimic this.

NOTE (2017-09-29): The problem described at the bugzilla page
referred to above, is most likely a case of someone not
understanding the proper usage of errno. We should review
whether "the problem is actually a problem" in PROJ.4 code.

Library specific allocators can be useful, and improve
interoperability, if properly used. That is, by making them
run/initialization time switchable, somewhat like the file i/o
interface.

But as things stand, we are more likely to get benefit
from reviewing the code for proper errno usage, which is hard,
due to the presence of context local and global pj_errnos.

Probably, these were introduced in order to support incomplete
implementations of thread local errnos at an early phase of the
implementation of multithreading support in PROJ.4).

It is likely too late to get rid of contexts, but we can still
benefit from a better usage of errno.
***********************************************************************/
    int old_errno = errno;
    void *res = malloc(size);
    if ( res && !old_errno )
            errno = 0;
    return res;
}


/**********************************************************************/
void *pj_calloc (size_t n, size_t size) {
/***********************************************************************
pj_calloc is the pj-equivalent of calloc().

It allocates space for an array of <n> elements of size <size>.
The array is initialized to zeros.
***********************************************************************/
    void *res = pj_malloc (n*size);
    if (0==res)
        return 0;
    memset (res, 0, n*size);
    return res;
}


/**********************************************************************/
void pj_dalloc(void *ptr) {
/**********************************************************************/
    free(ptr);
}


/**********************************************************************/
void *pj_dealloc (void *ptr) {
/***********************************************************************
pj_dealloc supports the common use case of "clean up and return a null
pointer" to signal an error in a multi level allocation:

    struct foo { int bar; int *baz; };

    struct foo *p = pj_calloc (1, sizeof (struct foo));
    if (0==p)
        return 0;

    p->baz = pj_calloc (10, sizeof(int));
    if (0==p->baz)
        return pj_dealloc (p); // clean up + signal error by 0-return

    return p;  // success

***********************************************************************/
    if (0==ptr)
        return 0;
    pj_dalloc (ptr);
    return 0;
}

/**********************************************************************/
char *pj_strdup(const char *str)
/**********************************************************************/
{
    size_t len = strlen(str) + 1;
    char *dup = pj_malloc(len);
    if (dup)
        memcpy(dup, str, len);
    return dup;
}


/*****************************************************************************/
void *pj_dealloc_params (PJ_CONTEXT *ctx, paralist *start, int errlev) {
/*****************************************************************************
    Companion to pj_default_destructor (below). Deallocates a linked list
    of "+proj=xxx" initialization parameters.

    Also called from pj_init_ctx when encountering errors before the PJ
    proper is allocated.
******************************************************************************/
    paralist *t, *n;
    for (t = start; t; t = n) {
        n = t->next;
        pj_dealloc(t);
    }
    pj_ctx_set_errno (ctx, errlev);
    return (void *) 0;
}




/************************************************************************/
/*                              pj_free()                               */
/*                                                                      */
/*      This is the application callable entry point for destroying     */
/*      a projection definition.  It does work generic to all           */
/*      projection types, and then calls the projection specific        */
/*      free function, P->destructor(), to do local work.               */
/*      In most cases P->destructor()==pj_default_destructor.           */
/************************************************************************/

void pj_free(PJ *P) {
    if (0==P)
        return;
    /* free projection parameters - all the hard work is done by */
    /* pj_default_destructor, which is supposed */
    /* to be called as the last step of the local destructor     */
    /* pointed to by P->destructor. In most cases,               */
    /* pj_default_destructor actually *is* what is pointed to    */
    P->destructor (P, proj_errno(P));
}




/*****************************************************************************/
void *pj_default_destructor (PJ *P, int errlev) {   /* Destructor */
/*****************************************************************************
    Does memory deallocation for "plain" PJ objects, i.e. that vast majority
    of PJs where the opaque object does not contain any additionally
    allocated memory below the P->opaque level.
******************************************************************************/

    /* Even if P==0, we set the errlev on pj_error and the default context   */
    /* Note that both, in the multithreaded case, may then contain undefined */
    /* values. This is expected behaviour. For MT have one ctx per thread    */
    if (0!=errlev)
        pj_ctx_set_errno (pj_get_ctx(P), errlev);

    if (0==P)
        return 0;

    /* free grid lists */
    pj_dealloc( P->gridlist );
    pj_dealloc( P->vgridlist_geoid );
    pj_dealloc( P->catalog_name );

    /* We used to call pj_dalloc( P->catalog ), but this will leak */
    /* memory. The safe way to clear catalog and grid is to call */
    /* pj_gc_unloadall(pj_get_default_ctx()); and pj_deallocate_grids(); */
    /* TODO: we should probably have a public pj_cleanup() method to do all */
    /* that */

    /* free the interface to Charles Karney's geodesic library */
    pj_dealloc( P->geod );

    /* free parameter list elements */
    pj_dealloc_params (pj_get_ctx(P), P->params, errlev);
    pj_dealloc (P->def_full);

    /* free the cs2cs emulation elements */
    pj_free (P->axisswap);
    pj_free (P->helmert);
    pj_free (P->cart);
    pj_free (P->cart_wgs84);
    pj_free (P->hgridshift);
    pj_free (P->vgridshift);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}
