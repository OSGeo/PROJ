/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of generic destructors, for memory deallocation
 *           for the large majority of PJ-objects that do not allocate
 *           anything alse than the PJ-object itself, and its associated
 *           opaque object.
 * Author:   Thomas Knudsen (2016)
 *
 ******************************************************************************
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


/*****************************************************************************/
static void *freeup_msg_plain (PJ *P, int errlev) {         /* Destructor */
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
static void freeup_plain (PJ *P) {
/*****************************************************************************
    Adapts freeup_msg_plain to the format defined for the PJ object. 
******************************************************************************/
    freeup_msg_plain (P, 0);
    return;
}




#if 0
/* not used - left here as demo for how to do projection specific freeups */
static void *freeup_msg_add (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;
    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    /* projection specific deallocation goes here */

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup_msg_add (PJ *P) {
    freeup_new_add (P, 0);
    return;
}
#endif
