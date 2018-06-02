/*
 * Copyright (c) 2014 Bojan Savric
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * The Patterson Cylindrical projection was designed by Tom Patterson, US National
 * Park Service, in 2014, using Flex Projector. The polynomial equations for the
 * projection were developed by Bojan Savric, Oregon State University, in
 * collaboration with Tom Patterson and Bernhard Jenny, Oregon State University.
 *
 * Java reference algorithm implemented by Bojan Savric in Java Map Projection
 * Library (a Java port of PROJ.4) in the file PattersonProjection.java.
 *
 * References:
 *    Java Map Projection Library
 *       https://github.com/OSUCartography/JMapProjLib
 *
 *    Patterson Cylindrical Projection
 *       http://shadedrelief.com/patterson/
 *
 *    Patterson, T., Savric, B., and Jenny, B. (2015). Cartographic Perspectives
 *       (No.78). Describes the projection design and characteristics, and
 *       developing the equations.    doi:10.14714/CP78.1270
 *       https://doi.org/10.14714/CP78.1270
 *
 * Port to PROJ.4 by Micah Cochran, 26 March 2016
 */

#define PJ_LIB__

#include <math.h>

#include "projects.h"

PROJ_HEAD(patterson, "Patterson Cylindrical") "\n\tCyl.";

#define K1 1.0148
#define K2 0.23185
#define K3 -0.14499
#define K4 0.02406
#define C1 K1
#define C2 (5.0 * K2)
#define C3 (7.0 * K3)
#define C4 (9.0 * K4)
#define EPS11 1.0e-11
#define MAX_Y 1.790857183
/* Not sure at all of the appropriate number for MAX_ITER... */
#define MAX_ITER 100


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    double phi2;
    (void) P;

    phi2 = lp.phi * lp.phi;
    xy.x = lp.lam;
    xy.y = lp.phi * (K1 + phi2 * phi2 * (K2 + phi2 * (K3 + K4 * phi2)));

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double yc, tol, y2, f, fder;
    int i;
    (void) P;

    yc = xy.y;

    /* make sure y is inside valid range */
    if (xy.y > MAX_Y) {
        xy.y = MAX_Y;
    } else if (xy.y < -MAX_Y) {
        xy.y = -MAX_Y;
    }

    for (i = MAX_ITER; i ; --i) { /* Newton-Raphson */
        y2 = yc * yc;
        f = (yc * (K1 + y2 * y2 * (K2 + y2 * (K3 + K4 * y2)))) - xy.y;
        fder = C1 + y2 * y2 * (C2 + y2 * (C3 + C4 * y2));
        yc -= tol = f / fder;
        if (fabs(tol) < EPS11) {
            break;
        }
    }
    if( i == 0 )
        pj_ctx_set_errno( P->ctx, PJD_ERR_NON_CONVERGENT );
    lp.phi = yc;

    /* longitude */
    lp.lam = xy.x;

    return lp;
}


PJ *PROJECTION(patterson) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
