/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement some service routines for the TRIPLET gerneric geodetic
 *           data type
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen/SDFE
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
#define PJ_TRIPLET_C
#include <proj.h>
#include <projects.h>



const TRIPLET trip_null = {{0,0,0}};
const TRIPLET trip_huge = {{HUGE_VAL,HUGE_VAL,HUGE_VAL}};


/* Direction: "+1" forward, "-1" reverse, 0: roundtrip precision check (not fully done) */
TRIPLET pj_apply (PJ *P, int direction, TRIPLET point) {
    TRIPLET p;
    double  d;

	switch (direction) {
	    case 1: 
            point.xyz  =  pj_fwd3d (point.lpz, P);
			return point;
        case -1:
            point.lpz  =  pj_inv3d (point.xyz, P);
			return point;
		case 0:
            p.xyz = pj_fwd3d (point.lpz, P);
            p.lpz = pj_inv3d (p.xyz, P);

		    /* Should add "both ways" here */
		    d = hypot (hypot(p.v[0] - point.v[0], p.v[1] - point.v[1]), p.v[2] - point.v[2]);
		    p.v[0] = p.v[1] = p.v[2] = d;
			return p;
		default:
	        pj_ctx_set_errno (P->ctx, EINVAL);
	}

    return trip_huge;
}


int pj_show_triplet (FILE *stream, const char *banner, TRIPLET point) {
    int i = 0;
    if (banner)
        fprintf (stream, "%s", banner);

    i += fprintf(stream,  "%16.10f ", point.xyz.x);
    i += fprintf(stream,  "%16.10f ", point.xyz.y);
    i += fprintf(stream,  "%16.4f",   point.xyz.z);
    if (banner)
        i +=     fprintf(stream, "\n");
    return i;
}
