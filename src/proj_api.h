/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Public (application) include file for PROJ.4 API, and constants.
 * Author:   Frank Warmerdam, <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 2001, Frank Warmerdam <warmerdam@pobox.com>
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
 * Revision 1.1  2001/04/06 01:24:22  warmerda
 * New
 *
 */

/* General projections header file */
#ifndef PROJ_API_H
#define PROJ_API_H

/* standard inclusions */
#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Try to update this every version! */
#define PJ_VERSION 443

extern char const pj_release[]; /* global release id string */

#define RAD_TO_DEG	57.29577951308232
#define DEG_TO_RAD	.0174532925199432958


extern int pj_errno;	/* global error return code */

#if !defined(PROJECTS_H)
    typedef struct { double u, v; } projUV;
    typedef void *projPJ;
    #define projXY projUV
    #define projLP projUV
#else
    typedef PJ *projPJ;
#   define projXY	XY
#   define projLP       LP
#endif

/* procedure prototypes */

projXY pj_fwd(projLP, projPJ);
projLP pj_inv(projXY, projPJ);

int pj_transform( projPJ src, projPJ dst, long point_count, int point_offset,
                  double *x, double *y, double *z );
int pj_datum_transform( projPJ src, projPJ dst, long point_count, int point_offset,
                        double *x, double *y, double *z );
int pj_geocentric_to_geodetic( double a, double ra,
                               long point_count, int point_offset,
                               double *x, double *y, double *z );
int pj_geodetic_to_geocentric( double a, double ra,
                               long point_count, int point_offset,
                               double *x, double *y, double *z );
int pj_compare_datums( projPJ srcdefn, projPJ dstdefn );
int pj_apply_gridshift( const char *, int, 
                        long point_count, int point_offset,
                        double *x, double *y, double *z );
void pj_deallocate_grids();
int pj_is_latlong(projPJ);
void pj_pr_list(projPJ);
void pj_free(projPJ);
projPJ pj_init(int, char **);
projPJ pj_init_plus(const char *);
projPJ pj_latlong_from_proj( projPJ );
void *pj_malloc(size_t);
void pj_dalloc(void *);
char *pj_strerrno(int);

#ifdef __cplusplus
}
#endif

#endif /* ndef PROJ_API_H */

