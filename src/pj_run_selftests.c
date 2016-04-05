/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Generic regression test for PROJ.4 projection algorithms.
 * Author:   Thomas Knudsen
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen
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


#include <stdio.h>
#define PJ_LIB__
#include <projects.h>


extern int pj_aea_selftest(void);
extern int pj_leac_selftest(void);



static void run_one_test (const char *mnemonic, int (testfunc)(void), int verbosity, int *n_ok, int *n_ko) {
    int ret = testfunc ();
    if (ret)
        (*n_ko)++;
    else
        (*n_ok)++;

    if (verbosity)
        printf ("Testing: %8s - return code: %d\n", mnemonic, ret);
}


int pj_run_selftests (int verbosity) {
    int n_ok = 0, n_ko = 0;

    if (verbosity)
       printf ("Running internal regression tests\n");
    run_one_test ("aea",    pj_aea_selftest,    verbosity, &n_ok, &n_ko);
    run_one_test ("leac",   pj_leac_selftest,   verbosity, &n_ok, &n_ko);

    if (0==verbosity)
       printf ("Internal regression tests done. ");
    printf ("Total: %d, Failure: %d, Success: %d\n", n_ok+n_ko, n_ko, n_ok);
    return n_ko;
}
