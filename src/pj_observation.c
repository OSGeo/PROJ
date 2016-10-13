/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement some service routines for the OBSERVATION generic
 *           geodetic data type
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-10-11
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
#include <proj.h>
#include <projects.h>


/* Constructor for the OBSERVATION object */
OBSERVATION pj_observation (
    double x, double y, double z, double t,
    double o, double p, double k,
    int id, unsigned int flags
) {
    OBSERVATION g;
    g.coo.xyzt.x = x;
    g.coo.xyzt.y = y;
    g.coo.xyzt.z = z;
    g.coo.xyzt.t = t;
    g.anc.opk.o  = o;
    g.anc.opk.p  = p;
    g.anc.opk.k  = k;
    g.id         = id;
    g.flags      = flags;
    return g;
}


/* Direction: "+1" forward, "-1" reverse, 0: roundtrip precision check (not fully done) */
OBSERVATION pj_apply (PJ *P, int direction, OBSERVATION obs) {
    double  d;
    OBSERVATION o;

    switch (direction) {
        case 1:
            obs.coo.xyz  =  pj_fwd3d (obs.coo.lpz, P);
            return obs;
        case -1:
            obs.coo.lpz  =  pj_inv3d (obs.coo.xyz, P);
            return obs;
        case 0:
            o.coo.xyz = pj_fwd3d (obs.coo.lpz, P);
            o.coo.lpz = pj_inv3d (o.coo.xyz, P);

            /* Should add "both ways" here */
            d = hypot (hypot (o.coo.v[0] - obs.coo.v[0], o.coo.v[1] - obs.coo.v[1]), o.coo.v[2] - obs.coo.v[2]);
            obs.coo.v[0] = obs.coo.v[1] = obs.coo.v[2] = d;
            return obs;
        default:
            pj_ctx_set_errno (P->ctx, EINVAL);
    }

    return pj_observation (HUGE_VAL,HUGE_VAL,HUGE_VAL,HUGE_VAL,HUGE_VAL,HUGE_VAL,HUGE_VAL,0,0);
}


int pj_show_triplet (FILE *stream, const char *banner, PJ_TRIPLET point) {
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
