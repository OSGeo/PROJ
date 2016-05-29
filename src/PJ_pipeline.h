/***********************************************************************

    Common header file for transformation pipeline manager functions

                 Thomas Knudsen, 2016-05-20

************************************************************************

Hopefully, this will be merged into projects.h or/and proj_api.h some
day, as it matures.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-20

************************************************************************
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
*
***********************************************************************/


#ifndef PJ_PIPELINE_H
#define PJ_PIPELINE_H


#ifdef __cplusplus
extern "C" {
#endif


/* View one type as the other. Definitely a hack - perhaps even a kludge */
/* Using a union is more appropriate (see below) */
#define ASXYZ(lpz) (*(XYZ *)(&lpz))
#define ASLPZ(xyz) (*(LPZ *)(&xyz))
#define ASXY(lp)   (*(XY  *)(&lp ))
#define ASLP(xy)   (*(LP  *)(&xy ))

/* Omega, Phi, Kappa: Rotations */
typedef struct {double o, p, k;} OPK;

/* Avoid explicit type-punning: Use a union */
typedef union {
    XYZ xyz;
    LPZ lpz;
    OPK opk;
    XY  xy;
    LP  lp;
} COORDINATE;


/* Apply the most appropriate projection function. No-op if none appropriate */
COORDINATE pj_apply_projection (COORDINATE point, int direction, PJ *P);

/* For debugging etc. */
int pj_show_coordinate (char *banner, COORDINATE point, int angular);
void pj_log_coordinate (projCtx ctx, int level, const char *banner, COORDINATE point, int angular);

int pj_is_pipeline (PJ *P);
int pj_pipeline_angular_output (PJ *P, int direction);
int pj_pipeline_angular_input (PJ *P, int direction);
int pj_pipeline_verbose (PJ *P);
int pj_pipeline_steps (PJ *P);
void pj_log_pipeline_steps (PJ *P, int level);



#ifdef __cplusplus
}
#endif

#endif /* end of basic projections header */
