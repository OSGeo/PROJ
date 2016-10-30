/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Memory management for proj.4.
 *           This version includes an implementation of generic destructors,
 *           for memory deallocation for the large majority of PJ-objects
 *           that do not allocate anything alse than the PJ-object itself,
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

#include <projects.h>
#include <errno.h>

/**********************************************************************/
void *pj_malloc(size_t size) {
/***********************************************************************
Currently, pj_malloc is a hack to solve an errno problem.
The problem is described in more details at
https://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=86420.
It seems, that pj_init and similar functions incorrectly
(under debian/glibs-2.3.2) assume that pj_malloc resets
errno after success. pj_malloc tries to mimic this.
***********************************************************************/
        int old_errno = errno;
        void *res = malloc(size);
        if ( res && !old_errno )
                errno = 0;
        return res;
}

/**********************************************************************/
void pj_dalloc(void *ptr) {
/**********************************************************************/
	free(ptr);
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




/*****************************************************************************/
void *pj_freeup_msg_plain (PJ *P, int errlev) {         /* Destructor */
/*****************************************************************************
    Does memory deallocation for "plain" PJ objects, i.e. that vast majority
    of PJs where the opaque object does not contain any additionally
    allocated memory.
******************************************************************************/
    if (0==P)
        return 0;

    if (0!=errlev)
        pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/*****************************************************************************/
void pj_freeup_plain (PJ *P) {
/*****************************************************************************
    Adapts pj_freeup_msg_plain to the format defined for the callback in
    the PJ object.

    i.e. reduces most instances of projection deallocation code to:

	static void freeup (PJ *P) {
 	    pj_freeup_plain (P);
 	    return;
 	}

	rather than:

 	static void *freeup_msg_add (PJ *P, int errlev) {
 	    if (0==P)
 	        return 0;
 	    pj_ctx_set_errno (P->ctx, errlev);

 	    if (0==P->opaque)
 	        return pj_dealloc (P);

 	    (* projection specific deallocation goes here *)

 	    pj_dealloc (P->opaque);
 	    return pj_dealloc(P);
 	}

 	(* Adapts pipeline_freeup to the format defined for the PJ object *)
 	static void freeup_msg_add (PJ *P) {
 	    freeup_new_add (P, 0);
 	    return;
 	}

 ******************************************************************************/
    pj_freeup_msg_plain (P, 0);
    return;
 }
