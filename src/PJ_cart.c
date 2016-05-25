/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Convert between ellipsoidal, geodetic coordinates and 
 *           cartesian, geocentric coordinates.
 *
 *           Formally, this functionality is also found in the PJ_geocent.c
 *           code.
 *
 *           Actually, however, the PJ_geocent transformations are carried
 *           out in concert between 2D stubs in PJ_geocent.c and 3D code
 *           placed in pj_transform.c.
 *
 *           For pipeline-style datum shifts, we do need direct access
 *           to the full 3D interface for this functionality.
 *
 *           Hence this code, which may look like "just another PJ_geocent"
 *           but really is something substantially different.
 *
 * Author:   Thomas Knudsen, thokn@sdfe.dk
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

#define PJ_LIB__
#include <projects.h>
#include <geocent.h>
#include <PJ_pipeline.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
PROJ_HEAD(cart,    "Ellipsoidal-to-cartesian");


/* Projection specific elements for the PJ object */
struct pj_opaque {
    GeocentricInfo g;
};




static COORDINATE cart_3d (COORDINATE point, int direction, PJ *P) {
    long ret;
	double X, Y, Z, LAM, PHI;
	COORDINATE result = {{HUGE_VAL, HUGE_VAL, HUGE_VAL}};

puts ("CART_3D");
    if (direction == 0) {
        ret = pj_Convert_Geodetic_To_Geocentric (
		    &(P->opaque->g),
		    point.lpz.phi,
		    point.lpz.lam,
		    point.lpz.z,
            &X,
            &Y,
            &Z
		);


        if (GEOCENT_NO_ERROR != ret)
	        return result;

		result.xyz.x = X;
		result.xyz.y = Y;
		result.xyz.z = Z;
		pj_show_coordinate ("cart: ", result, 0);

		return result;
	}

	pj_Convert_Geocentric_To_Geodetic (
	    &(P->opaque->g),
		point.xyz.x,
		point.xyz.y,
		point.xyz.z,
		&PHI,
		&LAM,
		&Z
	);
		
	result.lpz.lam = LAM;
	result.lpz.phi = PHI;
	result.lpz.z   =   Z;
	pj_show_coordinate ("ellp: ", result, 0);

    return result;
}




static XYZ cart_forward_3d (LPZ lpz, PJ *P) {
    COORDINATE point;
    point.lpz = lpz;
    pj_log( P->ctx, 5, "CART_FORWARD_3D");
    point = cart_3d (point, 0, P);
    return point.xyz;
}

static LPZ cart_reverse_3d (XYZ xyz, PJ *P) {
    COORDINATE point;
    point.xyz = xyz;
    pj_log( P->ctx, 5, "CART_REVERSE_3D");
    point = cart_3d (point, 1, P);
    return point.lpz;
}

static XY cart_forward (LP lp, PJ *P) {
    COORDINATE point;
    point.lp = lp;
    point.lpz.z = 0;
puts ("CART_FORWARD");

    point = cart_3d (point, 0, P);
puts ("CART_FORWARD");
    return point.xy;
}

static LP cart_reverse (XY xy, PJ *P) {
    COORDINATE point;
    point.xy = xy;
    point.xyz.z = 0;

    pj_log( P->ctx, 1, "CART_REVERSE");
    point = cart_3d (point, 1, P);
    return point.lp;
}




static void *cart_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup (PJ *P) {
    cart_freeup (P, 0);
    return;
}




PJ *PROJECTION(cart) {
    int err;
	double semiminor;
	COORDINATE result = {{0,0,0}};

    P->fwd3d = cart_forward_3d;
    P->inv3d = cart_reverse_3d;
    P->fwd   = cart_forward;
    P->inv   = cart_reverse;
puts ("CART");
    P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return 0;
		
    semiminor = P->a * sqrt (1 - P->es);
printf("semiminor=%12.3f\n", semiminor);
	err = pj_Set_Geocentric_Parameters (&(P->opaque->g), P->a, semiminor);
    if (GEOCENT_NO_ERROR != err)
	    return cart_freeup (P, -6); /* the "e effectively==1" message, abused as generic geocentric error */
puts ("CART OK");
pj_show_coordinate ("ellp: ", result, 0);

    return P;
}

/* selftest stub */
int pj_cart_selftest (void) {return 0;}
